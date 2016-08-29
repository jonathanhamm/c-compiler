#include "pp.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static cc_buf_s cc_pp_trigraph_lines(cc_buf_s src);
static cc_pp_toklist_s cc_pp_lex(cc_buf_s src);
static bool cc_pp_ishex(char c);
static bool cc_pp_isoct(char c);
static bool cc_pp_ishexquad(char *fptr);
static char *cc_pp_isunicn(char *fptr, unsigned lineno);

static char *cc_pp_identifier_suffix(char *fptr, unsigned lineno);
static char *cc_pp_identifier(char *fptr, unsigned lineno);

static void cc_pp_addtok(cc_pp_toklist_s *list, char *lexeme, cc_pp_toktype_e type, cc_pp_tokatt_e att);
static void cc_pp_printtok(cc_pp_toklist_s *list);

static bool cc_pp_is_hname(cc_pp_tok_s *tail);
static char *cc_pp_schar_seq(char *fptr, unsigned lineno);

cc_buf_s cc_pp_parse(cc_buf_s src) {
	cc_buf_s phase12;
	cc_pp_toklist_s list;

	phase12 = cc_pp_trigraph_lines(src);
	list = cc_pp_lex(phase12);
	cc_pp_printtok(&list);
	return phase12;
}


cc_buf_s cc_pp_trigraph_lines(cc_buf_s src) {
	char *ptr = src.buf;
	cc_buf_s result;
	
	cc_buf_init(&result);
	while(*ptr) {
		if(*ptr == '?' && *(ptr + 1) == '?') {
			switch(*(ptr + 2)) {
				case '=':
					cc_buf_str_addc(&result, '#');
					ptr += 3;
					break;
				case '(':
					cc_buf_str_addc(&result, '[');
					ptr += 3;
					break;
				case '/':
					if(*(ptr + 3) == '\n') {
						ptr += 4;
					}
					else {
						cc_buf_str_addc(&result, '\\');
						ptr += 3; 
					}
					break;
				case ')':
					cc_buf_str_addc(&result, ']');
					ptr += 3;
					break;
				case '\'':
					cc_buf_str_addc(&result, '^');
					ptr += 3;
					break;
				case '<':
					cc_buf_str_addc(&result, '{');
					ptr += 3;
					break;
				case '!':
					cc_buf_str_addc(&result, '|');
					ptr += 3;
					break;
				case '>':
					cc_buf_str_addc(&result, '}');
					ptr += 3;
					break;
				case '-':
					cc_buf_str_addc(&result, '~');
					ptr += 3;
					break;
				default:
					cc_buf_str_addstr(&result, "??", 2);
					cc_buf_str_addc(&result, *(ptr + 2));
					ptr += 3;
					break;
			}
		}
		else {
			if(*ptr == '\\' && *(ptr + 1) == '\n') {
				ptr += 2;	
			}
			cc_buf_str_addc(&result, *ptr);
			ptr++;
		}
	}
	//standard requires this, throw error if test passes?
	if(*(ptr - 1) != '\n') {
		cc_buf_str_addc(&result, '\n');
	}
	cc_buf_str_addc(&result, '\0');
	cc_buf_destroy(&src);
	return result;
}

cc_pp_toklist_s cc_pp_lex(cc_buf_s src) {
	char bck;
	char *ccheck;
	char *bptr = src.buf, *fptr = bptr;
	unsigned lineno = 1;
	cc_pp_toklist_s list = {
		.head = NULL,
		.tail = NULL
	};

	while(*fptr) {
		switch(*fptr) {
			case '\n':
				lineno++;
				cc_pp_addtok(&list, "lf", CCPP_TYPE_WS, CCPP_ATT_NEWLINE);
				fptr++;
				break;
			case ' ':
			case '\t':
			case '\v':
				do {
					fptr++;
				}
				while(*fptr == ' ' || *fptr == '\t' || *fptr == '\v');
				cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_DEFAULT);
				break;
			case '[':
				cc_pp_addtok(&list, "[", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
				fptr++;
				break;
			case ']':	
				cc_pp_addtok(&list, "]", CCPP_PUNCTUATOR, CCPP_ATT_RIGHT_BRACKET);
				fptr++;
				break;
			case '(':
				cc_pp_addtok(&list, "(", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_PAREN);
				fptr++;
				break;
			case ')':
				cc_pp_addtok(&list, ")", CCPP_PUNCTUATOR, CCPP_ATT_RIGHT_PAREN);
				fptr++;
				break;
			case '{':
				cc_pp_addtok(&list, "{", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACE);
				fptr++;
				break;
			case '}':
				cc_pp_addtok(&list, "}", CCPP_PUNCTUATOR, CCPP_ATT_RIGHT_BRACE);
				fptr++;
				break;
			case '.':
				if(*(fptr + 1) == '.' && *(fptr + 2) == '.') {
					cc_pp_addtok(&list, "...", CCPP_PUNCTUATOR, CCPP_ATT_VARARG);
					fptr += 3;
				}
				else {
					cc_pp_addtok(&list, ".", CCPP_PUNCTUATOR, CCPP_ATT_DOT);
					fptr++;
				}
				break;
			case '-':
				if(*(fptr + 1) == '>') {
					cc_pp_addtok(&list, "->", CCPP_PUNCTUATOR, CCPP_ATT_ARROW);
					fptr += 2;
				}
				else if(*(fptr + 1) == '-') {
					cc_pp_addtok(&list, "--", CCPP_PUNCTUATOR, CCPP_ATT_AUTODEC);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "-", CCPP_PUNCTUATOR, CCPP_ATT_MINUS);
					fptr++;
				}
				break;
			case '*':
				cc_pp_addtok(&list, "*", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
				fptr++;
				break;
			case '+':
				if(*(fptr + 1) == '+') {
					cc_pp_addtok(&list, "++", CCPP_PUNCTUATOR, CCPP_ATT_AUTOINC);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "+", CCPP_PUNCTUATOR, CCPP_ATT_PLUS);
					fptr++;
				}
				break;
			case '&':
				cc_pp_addtok(&list, "&", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
				fptr++;
				break;
			case '~':
				cc_pp_addtok(&list, "~", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
				fptr++;
				break;
			case '!':
				cc_pp_addtok(&list, "!", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
				fptr++;
				break;
			case '%':
				cc_pp_addtok(&list, "!", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
				fptr++;
				break;
			case '/':
				if(*(fptr + 1) == '/') {
					fptr += 2;
					while(*fptr != '\n')
						fptr++;
					cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_DEFAULT);
				}
				else if(*(fptr + 1) == '*') {
					fptr += 2;
					while(*fptr && !(*fptr == '*' && *(fptr + 1) == '/')) {
						if(*fptr == '\n') {
							lineno++;
							cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_NEWLINE);
						}
						fptr++;
					}
					cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_DEFAULT);
				}
				else {
					fptr++;
				}
				break;
			case '<':
				bptr = fptr;
				do {
					fptr++;
					switch(*fptr) {
						case '\'':
							cc_log_err("UDF - ' in h-char sequence.\n", "");
							break;
						case '\\':
							cc_log_err("UDF - \\ in h-char sequence.\n", "");
							break;

						case '/':
							if(*(fptr + 1) == '/') {
								cc_log_err("UDF - // in h-char sequence.\n", "");
							}
							else if(*(fptr + 1) == '*') {
								cc_log_err("UDF - /* in h-char sequence.\n", "");
							}
							break;
						case '\n':
							lineno++;
							cc_log_err("Illegal \\n in h-char sequence.\n", "");
							break;
						default:
							break;
					}
				} 
				while(*fptr && *fptr != '>');
				fptr++;
				bck = *fptr;
				*fptr = '\0';
				cc_pp_addtok(&list, bptr, CCPP_HEADER_NAME, CCPP_ATT_DEFAULT);
				*fptr = bck;
				break;
			case '"':
				if(cc_pp_is_hname(list.tail)) {
					bptr = fptr;
					do {
						fptr++;
						switch(*fptr) {
							case '\'':
								cc_log_err("UDF - ' in q-char sequence.\n", "");
								break;
							case '\\':
								cc_log_err("UDF - \\ in q-char sequence.\n", "");
								break;
							case '/':
								if(*(fptr + 1) == '/') {
									cc_log_err("UDF - // in q-char sequence.\n", "");
								}
								else if(*(fptr + 1) == '*') {
									cc_log_err("UDF - /* in q-char sequence.\n", "");
								}
								break;
							case '\n':
								lineno++;
								cc_log_err("Illegal \\n in q-char sequence.\n", "");
								break;
							default:
								break;
						}
					}
					while(*fptr && *fptr != '"');
					fptr++;
					bck = *fptr;
					*fptr = '\0';
					cc_pp_addtok(&list, bptr, CCPP_HEADER_NAME, CCPP_ATT_DEFAULT);
					*fptr = bck;
				}
				else {
					ccheck = cc_pp_schar_seq(fptr, lineno);		
					if(ccheck) {
						fptr = ccheck;
					}
					else {
						cc_log_err("Failed attempt to parse string literal.\n", "");
						fptr++;
					}
				}
				break;
			default:
				bptr = fptr;
				if((ccheck = cc_pp_identifier(fptr, lineno))) {
					bck = *ccheck;
					*ccheck = '\0';
					cc_pp_addtok(&list, bptr, CCPP_IDENTIFIER, CCPP_ATT_DEFAULT);
					*ccheck = bck;
					fptr = ccheck;
				}
				else if(isdigit(*fptr) || (*fptr == '.' && isdigit(*(fptr + 1)))) {
					while(true) {
						if(isdigit(*fptr) || *fptr == '.') {
							fptr++;	
						}
						else if((*fptr == 'e' || *fptr == 'E' || *fptr == 'p' || *fptr == 'P') 
								&& (*(fptr + 1) == '-' || *(fptr + 1) == '+')) {
							fptr += 2;
						}
						else if(isalpha(*fptr) || *fptr == '_') {
							fptr++;
						}
						else if((ccheck = cc_pp_isunicn(fptr, lineno))) {
							fptr = ccheck;
						}
						else {
							break;
						}
					}
					bck = *fptr;
					*fptr = '\0';
					cc_pp_addtok(&list, bptr, CCPP_NUMBER, CCPP_ATT_DEFAULT);
					*fptr = bck;
				}
				else {
					fptr++;
				}
				break;
		}
	}
	return list;
}

bool cc_pp_ishex(char c) {
	return (c >= '0' && c <= '9') || ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

bool cc_pp_isoct(char c) {
	return c >= '0' && c <= '7';
}

bool cc_pp_ishexquad(char *fptr) {
	return cc_pp_ishex(*fptr) && cc_pp_ishex(*(fptr + 1))
		&& cc_pp_ishex(*(fptr + 2)) && cc_pp_ishex(*(fptr + 3));
}

char *cc_pp_isunicn(char *fptr, unsigned lineno) {
	if(*fptr == '\\') {
		fptr++;
		if(*fptr == 'u') {
			if(cc_pp_ishexquad(fptr + 1)) {
				return fptr + 5;
			}
			else {
				cc_log_err("UDF - Invalid universal character.", "");
				return NULL;
			}
		}
		else if(*fptr == 'U') {
			if(cc_pp_ishexquad(fptr + 1) && cc_pp_ishexquad(fptr + 5)) {
				return fptr + 9;
			}
			else {
				cc_log_err("UDF - Invalid universal character.", "");
				return NULL;
			}
		}
	}
	return NULL;
}

char *cc_pp_identifier_suffix(char *fptr, unsigned lineno) {
	char *p;
	while(true) {
		if(isalpha(*fptr) || isdigit(*fptr) || *fptr == '_')
			fptr++;
		else if((p = cc_pp_isunicn(fptr, lineno))) {
			fptr = p;
		}
		else {
			return fptr;
		}
	}
}

char *cc_pp_identifier(char *fptr, unsigned lineno) {
	char *r = NULL;

	if(isalpha(*fptr) || *fptr == '_') {
		r = cc_pp_identifier_suffix(fptr + 1, lineno);
		
	}
	else if((r = cc_pp_isunicn(fptr, lineno))) {
		r = cc_pp_identifier_suffix(r, lineno); 
	}
	return r;
}

void cc_pp_addtok(cc_pp_toklist_s *list, char *lexeme, cc_pp_toktype_e type, cc_pp_tokatt_e att) {
	cc_pp_tok_s *t = cc_alloc(sizeof *t);

	t->lex = dupstr(lexeme);
	t->type = type;
	t->att = att;
	t->next = NULL;

	if(list->head) {
		t->prev = list->tail;
		list->tail->next = t;
	}
	else {
		t->prev = NULL;
		list->head = t;
	}
	list->tail = t;
}

void cc_pp_printtok(cc_pp_toklist_s *list) {
	cc_pp_tok_s *t;

	for(t = list->head; t; t = t->next) {
		printf("tok: %s %d %d\n", t->lex, t->type, t->att);
	}
}

bool cc_pp_is_hname(cc_pp_tok_s *tail) {
	if(tail) {
		if(tail->type == CCPP_TYPE_WS) { 
			do {
				if(tail->att == CCPP_ATT_NEWLINE) {
					return false;
				}
				else {
					tail = tail->prev;
				}
			}
			while(tail->type == CCPP_TYPE_WS);
			if(tail->type == CCPP_IDENTIFIER && (!strcmp(tail->lex, "include") || !strcmp(tail->lex, "pragma"))) {
				tail = tail->prev;
				if(tail->type == CCPP_PUNCTUATOR && tail->att == CCPP_ATT_HASH) {
					tail = tail->prev;
					while(tail && (tail->type == CCPP_TYPE_WS && tail->type != CCPP_ATT_NEWLINE)) {
						tail = tail->prev;
					}
					if(!tail || tail->type == CCPP_TYPE_WS) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

char *cc_pp_schar_seq(char *fptr, unsigned lineno) {
	char *ccheck; 
	while(*fptr && *fptr != '"') {
		if(*fptr == '\\') {
			switch(*(fptr + 1)) {
				case '\'':
					fptr++;
					break;
				case '"':
					fptr++;
					break;
				case '?':
					fptr++;
					break;
				case '\\':
					if(cc_pp_isoct(*(fptr + 1))) {
						fptr++;
						if(cc_pp_isoct(*(fptr + 2))) {
							fptr++;
							if(cc_pp_isoct(*(fptr + 3))) {
								fptr++;
							}
						}
					}
					else if(*(fptr + 1) == 'x') {
						if(cc_pp_ishex(*++fptr)) {
							while(cc_pp_ishex(*++fptr));
						}
						else {
							cc_log_err("UDF - \\x not followed by hex digits.", "");
						}
					}
					break;
				case 'a':
					fptr++;
					break;
				case 'b':
					fptr++;
					break;
				case 'f':
					fptr++;
					break;
				case 'n':
					fptr++;
					break;
				case 'r':
					fptr++;
					break;
				case 't':
					fptr++;
					break;
				case 'v':
					fptr++;
					break;
				default:;
					if((ccheck = cc_pp_isunicn(fptr - 1, lineno))) {
						fptr = ccheck;	
					}
					else {
						cc_log_err("UDF - Invalid Escape sequence in string literal.", "");
						fptr++;
					}
					break;
			}
		}
		else {
			fptr++;
		}
	}
	return NULL;
}

