#ifndef _cc_pp_h_
#define _cc_pp_h_

#include "general.h"

typedef struct cc_pp_tok_s cc_pp_tok_s;
typedef struct cc_pp_toklist_s cc_pp_toklist_s;

typedef enum cc_pp_toktype_e cc_pp_toktype_e; 
typedef enum cc_pp_tokatt_e cc_pp_tokatt_e;

enum cc_pp_toktype_e  {
	CCPP_TYPE_WS,
	CCPP_HEADER_NAME,
	CCPP_IDENTIFIER,
	CCPP_NUMBER,
	CCPP_CHARACTER_CONSTANT,
	CCPP_STRING_LITERAL,
	CCPP_PUNCTUATOR,
	CCPP_NON_WS
};

enum cc_pp_tokatt_e  {
	CCPP_ATT_DEFAULT,
	CCPP_ATT_NEWLINE,
	CCPP_ATT_LEFT_BRACKET,
	CCPP_ATT_RIGHT_BRACKET,
	CCPP_ATT_LEFT_PAREN,
	CCPP_ATT_RIGHT_PAREN,
	CCPP_ATT_LEFT_BRACE,
	CCPP_ATT_RIGHT_BRACE,
	CCPP_ATT_DOT,
	CCPP_ATT_VARARG,
	CCPP_ATT_ARROW,
	CCPP_ATT_AUTOINC,
	CCPP_ATT_AUTODEC,
	CCPP_ATT_AMP,
	CCPP_ATT_ASTERISK,
	CCPP_ATT_PLUS,
	CCPP_ATT_MINUS,
	CCPP_ATT_BITNOT,
	CCPP_ATT_LOGNOT,
	CCPP_ATT_DIV,
	CCPP_ATT_MOD,
	CCPP_ATT_LSHIFT,
	CCPP_ATT_RSHIFT,
	CCPP_ATT_LE,
	CCPP_ATT_GE,
	CCPP_ATT_LEQ,
	CCPP_ATT_GEQ,
	CCPP_ATT_EQ,
	CCPP_ATT_NEQ,
	CCPP_ATT_XOR,
	CCPP_ATT_BITOR,
	CCPP_ATT_LOGAND,
	CCPP_ATT_LOGOR,
	CCPP_ATT_ASSIGN,
	CCPP_ATT_ASSIGNMULT,
	CCPP_ATT_ASSIGNDIV,
	CCPP_ATT_ASSIGNMOD,
	CCPP_ATT_ASSIGNADD,
	CCPP_ATT_ASSIGNSUB,
	CCPP_ATT_ASSIGN_LSHIFT,
	CCPP_ATT_ASSIGN_RSHIFT,
	CCPP_ATT_ASSIGN_BITAND,
	CCPP_ATT_ASSIGN_XOR,
	CCPP_ATT_ASSIGN_BITOR,
	CCPP_ATT_COMMA,
	CCPP_ATT_HASH,
	CCPP_ATT_2HASH,
	CCPP_ATT_TERNARY,
	CCPP_ATT_COLON,
	CCPP_ATT_SEMICOLON

};

struct cc_pp_tok_s {
	cc_pp_toktype_e type;
	cc_pp_tokatt_e att;
	char *lex;
	cc_pp_tok_s *next, *prev;
};

struct cc_pp_toklist_s {
	cc_pp_tok_s *head;
	cc_pp_tok_s *tail;
};

extern cc_buf_s cc_pp_parse(cc_buf_s src);

#endif
