#include "keywords.h"
#include <string.h>

/**
 * Check if identifier token is actually a keyword
 *
 * - Currently using naive strcmp algorithm
 *
 */

cc_pp_tokatt_e cc_keyword_check(cc_pp_tok_s *tok) {
	char *lex = tok->lex;

	if(!strcmp(lex, "alignof")) {
		return CCPP_ATT_ALIGNOF;
	}
	else if(!strcmp(lex, "auto")) {
	}
	else if(!strcmp(lex, "break")) {
	}
	else if(!strcmp(lex, "case")) {
	}
	else if(!strcmp(lex, "char")) {
	}
	else if(!strcmp(lex, "const")) {
	}
	else if(!strcmp(lex, "continue")) {
	}
	else if(!strcmp(lex, "default")) {
	}
	else if(!strcmp(lex, "do")) {
	}
	else if(!strcmp(lex, "double")) {
	}
	else if(!strcmp(lex, "else")) {
	}
	else if(!strcmp(lex, "enum")) {
	}
	else if(!strcmp(lex, "extern")) {
	}
	else if(!strcmp(lex, "float")) {
	}
	else if(!strcmp(lex, "for")) {
	}
	else if(!strcmp(lex, "goto")) {
	}
	else if(!strcmp(lex, "if")) {
	}
	else if(!strcmp(lex, "inline")) {
	}
	else if(!strcmp(lex, "int")) {
	}
	else if(!strcmp(lex, "long")) {
	}
	else if(!strcmp(lex, "register")) {
	}
	else if(!strcmp(lex, "restrict")) {
	}
	else if(!strcmp(lex, "return")) {
	}
	else if(!strcmp(lex, "short")) {
	}
	else if(!strcmp(lex, "signed")) {
	}
	else if(!strcmp(lex, "sizeof")) {
	}
	else if(!strcmp(lex, "static")) {
	}
	else if(!strcmp(lex, "struct")) {
	}
	else if(!strcmp(lex, "switch")) {
	}
	else if(!strcmp(lex, "typedef")) {
	}
	else if(!strcmp(lex, "union")) {
	}
	else if(!strcmp(lex, "unsigned")) {
	}
	else if(!strcmp(lex, "void")) {
	}
	else if(!strcmp(lex, "volatile")) {
	}
	else if(!strcmp(lex, "while")) {
	}
	else if(!strcmp(lex, "_Alignas")) {
	}
	else if(!strcmp(lex, "_Atomic")) {
	}
	else if(!strcmp(lex, "_Bool")) {
	}
	else if(!strcmp(lex, "_Complex")) {
	}
	else if(!strcmp(lex, "_Generic")) {
	}
	else if(!strcmp(lex, "_Imaginary")) {
	}
	else if(!strcmp(lex, "_Noreturn")) {
	}
	else if(!strcmp(lex, "_Static_assert")) {
	}
	else if(!strcmp(lex, "_Thread_local")) {
	}
	else {
		return CCPP_ATT_DEFAULT;	
	}
}



