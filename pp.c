#include "pp.h"

static cc_buf_s cc_pp_trigraph_lines(cc_buf_s src);

cc_buf_s cc_pp_parse(cc_buf_s src) {
	cc_buf_s phase12;

	phase12 = cc_pp_trigraph_lines(src);
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
	cc_buf_str_addc(&result, '\0');
	cc_buf_destroy(&src);
	return result;
}
