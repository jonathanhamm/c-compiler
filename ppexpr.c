#include "ppexpr.h"
#include "keywords.h"

/*
 * Expression Grammar 
 *
 */


static void ccpp_primary_exression(cc_tok_s **tt);
static void ccpp_generic_selection(cc_tok_s **tt);
static void ccpp_generic_assoc_list(cc_tok_s **tt);
static void ccpp_generic_association(cc_tok_s **tt);
static void ccpp_postfix_expression(cc_tok_s **tt);
static void ccpp_argument_expression_list(cc_tok_s **tt);
static void ccpp_unary_expression(cc_tok_s **tt);
static void ccpp_unary_operator(cc_tok_s **tt);
static void ccpp_cast_expression(cc_tok_s **tt);
static void ccpp_multiplicative_expression(cc_tok_s **tt);
static void ccpp_additive_expression(cc_tok_s **tt);
static void ccpp_shift_expression(cc_tok_s **tt);
static void ccpp_relational_expression(cc_tok_s **tt);
static void ccpp_equality_expression(cc_tok_s **tt);
static void ccpp_and_expression(cc_tok_s **tt);
static void ccpp_exclusive_or_expression(cc_tok_s **tt);
static void ccpp_inclusive_or_expression(cc_tok_s **tt);
static void ccpp_logical_and_expression(cc_tok_s **tt);
static void ccpp_logical_or_expression(cc_tok_s **tt);
static void ccpp_conditional_expression(cc_tok_s **tt);
static void ccpp_assignment_expression(cc_tok_s **tt);
static void ccpp_assignment_operator(cc_tok_s **tt);
static void ccpp_expression(cc_tok_s **tt);

cc_toklist_s pptok_to_cctok(cc_tok_s *ppt) {
	cc_toklist_s list = {NULL, NULL};
	cc_tokatt_e att_id;

	switch(ppt->type) {
		case CCPP_TYPE_WS:
			return list;
		case CCPP_IDENTIFIER:
			att_id = cc_keyword_check(ppt);
			if(att_id == CCPP_ATT_NONE) {
			}
			else {
				ppt->type = CCPP_KEYWORD;
				ppt->att = att_id;
			}
			break;
		default:
			break;
	}
	return list;
}

/*
 *
 **************** FUNCTION DEFINITIONS ***************
 *
 */


void ccpp_primary_exression(cc_tok_s **tt) {
	cc_tok_s *t = *tt;

	switch(t->type) {
		case CCPP_IDENTIFIER:
			break;
		case CCPP_CONSTANT:
			break;
		case CCPP_STRING_LITERAL:
			break;
		case CCPP_PUNCTUATOR:
			break;
		default:
			break;
	}
}

void ccpp_generic_selection(cc_tok_s **tt) {
}

void ccpp_generic_assoc_list(cc_tok_s **tt) {
}

void ccpp_generic_association(cc_tok_s **tt) {
}

void ccpp_postfix_expression(cc_tok_s **tt) {
}

void ccpp_argument_expression_list(cc_tok_s **tt) {
}

void ccpp_unary_expression(cc_tok_s **tt) {
}

void ccpp_unary_operator(cc_tok_s **tt) {
}

void ccpp_cast_expression(cc_tok_s **tt) {
}

void ccpp_multiplicative_expression(cc_tok_s **tt) {
}

void ccpp_additive_expression(cc_tok_s **tt) {
}

void ccpp_shift_expression(cc_tok_s **tt) {
}

void ccpp_relational_expression(cc_tok_s **tt) {
}

void ccpp_equality_expression(cc_tok_s **tt) {
}

void ccpp_and_expression(cc_tok_s **tt) {
}

void ccpp_exclusive_or_expression(cc_tok_s **tt) {
}

void ccpp_inclusive_or_expression(cc_tok_s **tt) {
}

void ccpp_logical_and_expression(cc_tok_s **tt) {
}

void ccpp_logical_or_expression(cc_tok_s **tt) {
}

void ccpp_conditional_expression(cc_tok_s **tt) {
}

void ccpp_assignment_expression(cc_tok_s **tt) {
}

void ccpp_assignment_operator(cc_tok_s **tt) {
}

void ccpp_expression(cc_tok_s **tt) {
}

void ccpp_constant_expression(cc_tok_s **tt) {
}
