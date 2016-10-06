#ifndef _cc_lex_h_
#define _cc_lex_h_

#include "pp.h"

extern cc_toklist_s pptok_to_cctok(cc_tok_s *ppt);

extern void ccpp_constant_expression(cc_tok_s **t);

#endif

