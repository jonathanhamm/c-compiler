#include "pp.h"
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

static cc_buf_s cc_pp_trigraph_lines(cc_buf_s src);
static cc_pp_toklist_s cc_pp_lex(cc_buf_s src);
static bool cc_pp_ishex(char c);
static bool cc_pp_ishexquad(char *fptr);
static char *cc_pp_isunicn(char *fptr);

static char *cc_pp_identifier_suffix();
static char *cc_pp_identifier(char *fptr);

static void cc_pp_addtok(cc_pp_toklist_s *list, char *lexeme, cc_pp_toktype_e type, cc_pp_tokatt_e att);
static void cc_pp_printtok(cc_pp_toklist_s *list);

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
	cc_pp_toklist_s list = {
		.head = NULL,
		.tail = NULL
	};

	while(*fptr) {
		switch(*fptr) {
			case '\n':
				cc_pp_addtok(&list, "lf", CCPP_TYPE_WS, CCPP_ATT_NEWLINE);
				fptr++;
				break;
			case ' ':
			case '\t':
			case '\v':
				do {
					fptr++;
				}
				while( *fptr == ' ' || *fptr == '\t' || *fptr == '\v');
				cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_DEFAULT);
				break;
			case '/':
				if(*(fptr + 1) == '/') {
					fptr += 2;
					while(*fptr != '\n')
						fptr++;
					cc_pp_addtok(&list, " ", CCPP_TYPE_WS, CCPP_ATT_DEFAULT);
				}
				else if(*(fptr + 1) == '*') {
					do {
						fptr++;
					}
					while(!(*fptr == '*' && *(fptr + 1) == '/'));
				}
				break;
			default:
				bptr = fptr;
				if((ccheck = cc_pp_identifier(fptr))) {
					bck = *ccheck;
					*fptr = '\0';
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
						else if((ccheck = cc_pp_isunicn(fptr))) {
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
					fptr = bptr;
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

bool cc_pp_ishexquad(char *fptr) {
	return cc_pp_ishex(*fptr) && cc_pp_ishex(*(fptr + 1))
		&& cc_pp_ishex(*(fptr + 2)) && cc_pp_ishex(*(fptr + 3));
}

char *cc_pp_isunicn(char *fptr) {
	if(*fptr == '\\') {
		fptr++;
		if(*fptr == 'u') {
			if(cc_pp_ishexquad(fptr + 1)) {
				return fptr + 5;
			}
			else {
				return NULL;
			}
		}
		else if(*fptr == 'U') {
			if(cc_pp_ishexquad(fptr + 1) && cc_pp_ishexquad(fptr + 5)) {
				return fptr + 9;
			}
			else {
				return NULL;
			}
		}
	}
	return NULL;
}

char *cc_pp_identifier_suffix(char *fptr) {
	char *p;
	while(true) {
		if(isalpha(*fptr) || isdigit(*fptr) || *fptr == '_')
			fptr++;
		else if((p = cc_pp_isunicn(fptr))) {
			fptr = p;
		}
		else {
			return fptr;
		}
	}
}

char *cc_pp_identifier(char *fptr) {
	char *r = NULL;

	if(isalpha(*fptr) || *fptr == '_') {
		r = cc_pp_identifier_suffix(fptr + 1);
		
	}
	else if((r = cc_pp_isunicn(fptr))) {
		r = cc_pp_identifier_suffix(r); 
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
		list->tail->next = t;
	}
	else {
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

