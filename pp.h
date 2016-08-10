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
	CCPP_CHARACTER_CONTSTANT,
	CCPP_STRING_LITERAL,
	CCPP_PUNCTUATOR,
	CCPP_NON_WS
};

enum cc_pp_tokatt_e  {
	CCPP_ATT_DEFAULT,
	CCPP_ATT_NEWLINE
};

struct cc_pp_tok_s {
	cc_pp_toktype_e type;
	cc_pp_tokatt_e att;
	char *lex;
	unsigned lineno;
	cc_pp_tok_s *next;
};

struct cc_pp_toklist_s {
	cc_pp_tok_s *head;
	cc_pp_tok_s *tail;
};

extern cc_buf_s cc_pp_parse(cc_buf_s src);

#endif
