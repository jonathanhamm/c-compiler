#include "keywords.h"
#include <string.h>


/**
 * Check if identifier token is actually a keyword
 *
 * - Currently using naive strcmp algorithm
 *
 */

cc_tokatt_e cc_keyword_check(cc_tok_s *tok) {
	char *lex = tok->lex;

	if(!strcmp(lex, "alignof")) {
		return CCPP_ATT_ALIGNOF;
	}
	else if(!strcmp(lex, "auto")) {
		return CCPP_ATT_AUTO;
	}
	else if(!strcmp(lex, "break")) {
		return CCPP_ATT_BREAK;
	}
	else if(!strcmp(lex, "case")) {
		return CCPP_ATT_CASE;
	}
	else if(!strcmp(lex, "char")) {
		return CCPP_ATT_CHAR;
	}
	else if(!strcmp(lex, "const")) {
		return CCPP_ATT_CONST;
	}
	else if(!strcmp(lex, "continue")) {
		return CCPP_ATT_CONTINUE;
	}
	else if(!strcmp(lex, "default")) {
		return CCPP_ATT_DEFAULT;
	}
	else if(!strcmp(lex, "do")) {
		return CCPP_ATT_DO;
	}
	else if(!strcmp(lex, "double")) {
		return CCPP_ATT_DOUBLE;
	}
	else if(!strcmp(lex, "else")) {
		return CCPP_ATT_ELSE;
	}
	else if(!strcmp(lex, "enum")) {
		return CCPP_ATT_ENUM;
	}
	else if(!strcmp(lex, "extern")) {
		return CCPP_ATT_EXTERN;
	}
	else if(!strcmp(lex, "float")) {
		return CCPP_ATT_FLOAT;
	}
	else if(!strcmp(lex, "for")) {
		return CCPP_ATT_FOR;
	}
	else if(!strcmp(lex, "goto")) {
		return CCPP_ATT_GOTO;
	}
	else if(!strcmp(lex, "if")) {
		return CCPP_ATT_IF;
	}
	else if(!strcmp(lex, "inline")) {
		return CCPP_ATT_INLINE;
	}
	else if(!strcmp(lex, "int")) {
		return CCPP_ATT_INT;
	}
	else if(!strcmp(lex, "long")) {
		return CCPP_ATT_LONG;
	}
	else if(!strcmp(lex, "register")) {
		return CCPP_ATT_REGISTER;
	}
	else if(!strcmp(lex, "restrict")) {
		return CCPP_ATT_RESTRICT;
	}
	else if(!strcmp(lex, "return")) {
		return CCPP_ATT_RETURN;
	}
	else if(!strcmp(lex, "short")) {
		return CCPP_ATT_SHORT;
	}
	else if(!strcmp(lex, "signed")) {
		return CCPP_ATT_SIGNED;
	}
	else if(!strcmp(lex, "sizeof")) {
		return CCPP_ATT_SIZEOF;
	}
	else if(!strcmp(lex, "static")) {
		return CCPP_ATT_STATIC;
	}
	else if(!strcmp(lex, "struct")) {
		return CCPP_ATT_STRUCT;
	}
	else if(!strcmp(lex, "switch")) {
		return CCPP_ATT_SWITCH;
	}
	else if(!strcmp(lex, "typedef")) {
		return CCPP_ATT_TYPEDEF;
	}
	else if(!strcmp(lex, "union")) {
		return CCPP_ATT_UNION;
	}
	else if(!strcmp(lex, "unsigned")) {
		return CCPP_ATT_UNSIGNED;
	}
	else if(!strcmp(lex, "void")) {
		return CCPP_ATT_VOID;
	}
	else if(!strcmp(lex, "volatile")) {
		return CCPP_ATT_VOLATILE;
	}
	else if(!strcmp(lex, "while")) {
		return CCPP_ATT_WHILE;
	}
	else if(!strcmp(lex, "_Alignas")) {
		return CCPP_ATT__ALIGNAS;
	}
	else if(!strcmp(lex, "_Atomic")) {
		return CCPP_ATT__ATOMIC;
	}
	else if(!strcmp(lex, "_Bool")) {
		return CCPP_ATT__BOOL;
	}
	else if(!strcmp(lex, "_Complex")) {
		return CCPP_ATT__COMPLEX;
	}
	else if(!strcmp(lex, "_Generic")) {
		return CCPP_ATT__GENERIC;
	}
	else if(!strcmp(lex, "_Imaginary")) {
		return CCPP_ATT__IMAGINARY;
	}
	else if(!strcmp(lex, "_Noreturn")) {
		return CCPP_ATT__IMAGINARY;
	}
	else if(!strcmp(lex, "_Static_assert")) {
		return CCPP_ATT__STATIC_ASSERT;
	}
	else if(!strcmp(lex, "_Thread_local")) {
		return CCPP_ATT__THREAD_LOCAL; }
	else {
		return CCPP_ATT_NONE;	
	}
}



