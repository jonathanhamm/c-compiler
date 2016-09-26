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
static char *cc_pp_escape_seq(char *fptr, unsigned lineno);
static char *cc_pp_schar_seq(char *fptr, unsigned lineno);
static char *cc_pp_cchar_seq(char *fptr, unsigned lineno);

static void cc_pp(cc_pp_toklist_s list);
static bool cc_pp_ws_seq(cc_pp_tok_s **tt);
static cc_pp_tok_s *cc_pp_if_section(cc_pp_tok_s *t);
static void cc_pp_if_group(cc_pp_tok_s **tt);
static void cc_pp_elif_groups(cc_pp_tok_s **tt);
static void cc_pp_elif_group(cc_pp_tok_s **tt);
static void cc_pp_else_groupe(cc_pp_tok_s **tt);
static void cc_pp_endif_line(cc_pp_tok_s **tt);
static void cc_pp_control_line(cc_pp_tok_s **tt);
static void cc_pp_text_line(cc_pp_tok_s **tt);
static void cc_pp_non_directive(cc_pp_tok_s **tt);
static void cc_pp_lparen(cc_pp_tok_s **tt);
static void cc_pp_replacement_list(cc_pp_tok_s **tt);
static void cc_pp_pp_tokens(cc_pp_tok_s **tt);
static void cc_pp_new_line(cc_pp_tok_s **tt);
static void cc_pp_define(cc_pp_tok_s **tt);

cc_buf_s cc_pp_parse(cc_buf_s src) {
	cc_buf_s phase12;
	cc_pp_toklist_s list;

	phase12 = cc_pp_trigraph_lines(src);
	list = cc_pp_lex(phase12);
	cc_pp(list);
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
				cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_NONE);
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
				if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "*=", CCPP_PUNCTUATOR, CCPP_ATT_ASSIGNMULT);
					fptr+=2;
				}
				else {
					cc_pp_addtok(&list, "*", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
					fptr++;
				}
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
				if(*(fptr + 1) == '&') {
					cc_pp_addtok(&list, "&", CCPP_PUNCTUATOR, CCPP_ATT_LOGAND);
				}
				else if(*(fptr + 1) == '=') {
				}
				else {
					cc_pp_addtok(&list, "&", CCPP_PUNCTUATOR, CCPP_ATT_AMP);
					fptr++;
				}
				break;
			case '~':
				cc_pp_addtok(&list, "~", CCPP_PUNCTUATOR, CCPP_ATT_BITNOT);
				fptr++;
				break;
			case '!':
				if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "!=", CCPP_PUNCTUATOR, CCPP_ATT_NEQ);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "!", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
					fptr++;
				}
				break;
			case '%':
				if(*(fptr + 1) == '>') {
					cc_pp_addtok(&list, "}", CCPP_PUNCTUATOR, CCPP_ATT_RIGHT_BRACE);
					fptr += 2;
				}
				else if(*(fptr + 1) == ':') {
					if(*(fptr + 2) == '%' && *(fptr + 3) == ':') {
						cc_pp_addtok(&list, "##", CCPP_PUNCTUATOR, CCPP_ATT_2HASH);
						fptr += 4;
					}
					else {
						cc_pp_addtok(&list, "#", CCPP_PUNCTUATOR, CCPP_ATT_HASH);
						fptr += 2;
					}
				}
				else {
					cc_pp_addtok(&list, "%", CCPP_PUNCTUATOR, CCPP_ATT_MOD);
					fptr++;
				}	
				break;
			case '/':
				if(*(fptr + 1) == '/') {
					fptr += 2;
					while(*fptr != '\n')
						fptr++;
					cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_NONE);
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
					cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_NONE);
				}
				else if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "/=", CCPP_PUNCTUATOR, CCPP_ATT_ASSIGNDIV);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "/", CCPP_PUNCTUATOR, CCPP_ATT_DIV);
					fptr++;
				}
				break;
			case '<':
				if(cc_pp_is_hname(list.tail)) {
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
					cc_pp_addtok(&list, bptr, CCPP_HEADER_NAME, CCPP_ATT_NONE);
					*fptr = bck;
				}
				else if(*(fptr + 1) == '<') {
					cc_pp_addtok(&list, "<<", CCPP_PUNCTUATOR, CCPP_ATT_LSHIFT);
					fptr += 2;
				}
				else if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "<=", CCPP_PUNCTUATOR, CCPP_ATT_LEQ);
					fptr += 2;
				}
				else if(*(fptr + 1) == ':') {
					cc_pp_addtok(&list, "[", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACKET);
					fptr += 2;
				}
				else if(*(fptr + 1) == '%') {
					cc_pp_addtok(&list, "{", CCPP_PUNCTUATOR, CCPP_ATT_LEFT_BRACE);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "<", CCPP_PUNCTUATOR, CCPP_ATT_LE);
					fptr++;
				}
				break;
			case '>':
				if(*(fptr + 1) == '>') {
					cc_pp_addtok(&list, ">>", CCPP_PUNCTUATOR, CCPP_ATT_RSHIFT);
					fptr += 2;
				}
				else if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, ">=", CCPP_PUNCTUATOR, CCPP_ATT_GE);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, ">", CCPP_PUNCTUATOR, CCPP_ATT_GE);
					fptr++;
				}
				break;
			case '=':
				if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "==", CCPP_PUNCTUATOR, CCPP_ATT_EQ);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, ">", CCPP_PUNCTUATOR, CCPP_ATT_ASSIGN);
					fptr++;
				}
				break;
			case '^':
				if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "^=", CCPP_PUNCTUATOR, CCPP_ATT_ASSIGN_XOR);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "^", CCPP_PUNCTUATOR, CCPP_ATT_XOR);
					fptr++;
				}
				break;
			case '|':
				if(*(fptr + 1) == '|') {
					cc_pp_addtok(&list, "||", CCPP_PUNCTUATOR, CCPP_ATT_LOGOR);
					fptr += 2;
				}
				else if(*(fptr + 1) == '=') {
					cc_pp_addtok(&list, "|=", CCPP_PUNCTUATOR, CCPP_ATT_ASSIGN_BITOR);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, "|", CCPP_PUNCTUATOR, CCPP_ATT_BITOR);
					fptr++;
				}
				break;
			case '?':
				cc_pp_addtok(&list, "?", CCPP_PUNCTUATOR, CCPP_ATT_TERNARY);
				fptr++;
				break;
			case ':':
				if(*(fptr + 1) == '>') {
					cc_pp_addtok(&list, "]", CCPP_PUNCTUATOR, CCPP_ATT_RIGHT_BRACKET);
					fptr += 2;
				}
				else {
					cc_pp_addtok(&list, ":", CCPP_PUNCTUATOR, CCPP_ATT_COLON);
					fptr++;
				}
				break;
			case ';':
				cc_pp_addtok(&list, ";", CCPP_PUNCTUATOR, CCPP_ATT_SEMICOLON);
				fptr++;
				break;
			case ',':
				cc_pp_addtok(&list, ",", CCPP_PUNCTUATOR, CCPP_ATT_COMMA);
				fptr++;
				break;
			case '#':
				if(*(fptr + 1) == '#') {
					cc_pp_addtok(&list, "##", CCPP_PUNCTUATOR, CCPP_ATT_2HASH);
					fptr +=2;
				}
				else {
					cc_pp_addtok(&list, "#", CCPP_PUNCTUATOR, CCPP_ATT_HASH);
					fptr++;
				}
				break;
			case 'u':
				bptr = fptr;
				if(*(fptr + 1) == '8') {
					fptr++;
				}
				if(*(fptr + 1) == '"') {
					ccheck = cc_pp_schar_seq(fptr + 2, lineno);
					if(ccheck) {
						fptr = ccheck;
						bck = *fptr;
						*fptr = '\0';
						cc_pp_addtok(&list, bptr, CCPP_STRING_LITERAL, CCPP_ATT_NONE);
						*fptr = bck;
					}
					else {
						cc_log_err("Failed attempt to parse string literal.\n", "");
						fptr++;
					}
				}
				else if(*(fptr + 1) == '\'') {
					ccheck = cc_pp_cchar_seq(fptr + 2, lineno);
					if(ccheck) {
						fptr = ccheck;
						bck = *fptr;
						*fptr = '\0';
						cc_pp_addtok(&list, bptr, CCPP_CHARACTER_CONSTANT, CCPP_ATT_NONE);
						*fptr = bck;
					}
					else {
						cc_log_err("Failed attempt to parse string literal.\n", "");
						fptr++;
					}
				}
				else {
					if(*fptr == '8')
						fptr--;
					goto ccpp_default;
				}
				break;
			case 'U':
			case 'L':
				if(*(fptr + 1) == '"') {
					bptr = fptr;
					ccheck = cc_pp_schar_seq(fptr + 1, lineno);		
					if(ccheck) {
						fptr = ccheck;
						bck = *fptr;
						*fptr = '\0';
						cc_pp_addtok(&list, bptr, CCPP_STRING_LITERAL, CCPP_ATT_NONE);
						*fptr = bck;
					}
					else {
						cc_log_err("Failed attempt to parse string literal.\n", "");
						fptr++;
					}
				}
				else if(*(fptr + 1) == '\'') {
					bptr = fptr;
					ccheck = cc_pp_cchar_seq(fptr + 1, lineno);
					if(ccheck) {
						fptr = ccheck;
						bck = *fptr;
						*fptr = '\0';
						cc_pp_addtok(&list, bptr, CCPP_CHARACTER_CONSTANT, CCPP_ATT_NONE);
						*fptr = bck;
					}
					else {
						cc_log_err("Failed attempt to parse character constant.\n", "");
						fptr++;
					}
				}
				else {
					goto ccpp_default; 
				}
				break;
			case '\'':
				bptr = fptr;
				ccheck = cc_pp_cchar_seq(fptr + 1, lineno);
				if(ccheck) {
					fptr = ccheck;
					bck = *fptr;
					*fptr = '\0';
					cc_pp_addtok(&list, bptr, CCPP_CHARACTER_CONSTANT, CCPP_ATT_NONE);
					*fptr = bck;
				}
				else {
					cc_log_err("Failed attempt to parse character constant.\n", "");
					fptr++;
				}
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
					cc_pp_addtok(&list, bptr, CCPP_HEADER_NAME, CCPP_ATT_NONE);
					*fptr = bck;
				}
				else {
					bptr = fptr;
					ccheck = cc_pp_schar_seq(fptr + 1, lineno);		
					if(ccheck) {
						fptr = ccheck;
						bck = *fptr;
						*fptr = '\0';
						cc_pp_addtok(&list, bptr, CCPP_STRING_LITERAL, CCPP_ATT_NONE);
						*fptr = bck;
					}
					else {
						cc_log_err("Failed attempt to parse string literal.\n", "");
						fptr++;
					}
				}
				break;
			default:
ccpp_default:
				bptr = fptr;
				if((ccheck = cc_pp_identifier(fptr, lineno))) {
					bck = *ccheck;
					*ccheck = '\0';
					cc_pp_addtok(&list, bptr, CCPP_IDENTIFIER, CCPP_ATT_NONE);
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
					cc_pp_addtok(&list, bptr, CCPP_NUMBER, CCPP_ATT_NONE);
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

char *cc_pp_escape_seq(char *fptr, unsigned lineno) {
	char *ccheck;
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
	return fptr;
}

char *cc_pp_schar_seq(char *fptr, unsigned lineno) {
	while(*fptr && *fptr != '"') {
		if(*fptr == '\\') {
			fptr = cc_pp_escape_seq(fptr, lineno);
		}
		else {
			fptr++;
		}
	}
	return fptr + 1;
}

char *cc_pp_cchar_seq(char *fptr, unsigned lineno) {
	while(*fptr && *fptr != '\'') {
		if(*fptr == '\\') {
			fptr = cc_pp_escape_seq(fptr, lineno);	
		}
		else {
			fptr++;
		}
	}
	return fptr + 1;
}

void cc_pp(cc_pp_toklist_s list) {
	cc_pp_tok_s *t = list.head, *check;

	while(t) {
		check = cc_pp_if_section(t);
		if(!check) {
		}
	}
}

bool cc_pp_ws_seq(cc_pp_tok_s **tt) {
	bool gotlf = false;
	cc_pp_tok_s *t = *tt;

	while(t->type == CCPP_TYPE_WS) {
		if(t->att == CCPP_ATT_NEWLINE) {
			gotlf = true;
		}
	}
	*tt = t;
	return gotlf;
}

cc_pp_tok_s *cc_pp_if_section(cc_pp_tok_s *t) {
	if(t->type == CCPP_PUNCTUATOR) {
		if(t->att == CCPP_ATT_HASH) {
			if(!strcmp(t->lex, "if")) {
				
			}
			else if(!strcmp(t->lex, "ifdef")) {
			}
			else if(!strcmp(t->lex, "ifndef")) {
			}
		} 
	}
	
	return NULL;
}

void cc_pp_if_group(cc_pp_tok_s **tt) {
		
}

void cc_pp_elif_groups(cc_pp_tok_s **tt) {
}

void cc_pp_elif_group(cc_pp_tok_s **tt) {
}

void cc_pp_else_groupe(cc_pp_tok_s **tt) {
}

void cc_pp_endif_line(cc_pp_tok_s **tt) {
}

void cc_pp_control_line(cc_pp_tok_s **tt) {
	cc_pp_tok_s *t = *tt;

	t = t->next;

	if(!strcmp(t->lex, "include")) {

	}
	else if(!strcmp(t->lex, "define")) {
		*tt = t;
		cc_pp_define(tt);
	}
	else if(!strcmp(t->lex, "undef")) {
	}
	else if(!strcmp(t->lex, "line")) {
	}
	else if(!strcmp(t->lex, "error")) {
	}
	else if(!strcmp(t->lex, "pragma")) {
	}
	else if(t->type == CCPP_TYPE_WS && t->att == CCPP_ATT_NEWLINE) {
	}
	else {
		//syntax error: invalid pp directive
	}
}

void cc_pp_text_line(cc_pp_tok_s **tt) {
}

void cc_pp_non_directive(cc_pp_tok_s **tt) {
}

void cc_pp_lparen(cc_pp_tok_s **tt) {
}

void cc_pp_replacement_list(cc_pp_tok_s **tt) {
}

void cc_pp_pp_tokens(cc_pp_tok_s **tt) {
}

void cc_pp_new_line(cc_pp_tok_s **tt) {
}

void cc_pp_define(cc_pp_tok_s **tt) {
	cc_pp_tok_s *t = *tt;

	t = t->next;

	if(t->type == CCPP_IDENTIFIER) {
		t = t->next;

	}
	else {
		//syntax error: Expected identifier after #define directive
	}
}

