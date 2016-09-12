#include "parse.h"

/*
 * Expression Grammar 
 *
 */

static void ccp_primary_exression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_generic_selection(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_generic_assoc_list(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_generic_association(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_postfix_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_argument_expression_list(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_unary_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_unary_operator(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_cast_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_multiplicative_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_additive_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_shift_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_relational_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_equality_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_and_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_exclusive_or_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_inclusive_or_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_logical_and_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_logical_or_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_conditional_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_assignment_expression(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_assignment_operator(cc_pp_tok_s **tt, bool mode_pp);
static void ccp_expression(cc_pp_tok_s **tt, bool mode_pp);


/*
 *
 **************** FUNCTION DEFINITIONS ***************
 *
 */


void ccp_primary_exression(cc_pp_tok_s **tt, bool mode_pp) {
	cc_pp_tok_s *t = *tt;

	switch(t->type) {
		case CCPP_IDENTIFIER:
			if(mode_pp) {
			}
			else {

			}
			break;
		case CCPP_CONSTANT:
			break;
		case CCPP_STRING_LITERAL:
			break;
		case CCPP_PUNCTUATOR:
			break;
		case CCPP_KEYWORD:
			break;
		default:
			break;
	}
}

void ccp_generic_selection(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_generic_assoc_list(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_generic_association(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_postfix_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_argument_expression_list(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_unary_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_unary_operator(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_cast_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_multiplicative_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_additive_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_shift_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_relational_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_equality_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_and_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_exclusive_or_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_inclusive_or_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_logical_and_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_logical_or_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_conditional_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_assignment_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_assignment_operator(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_expression(cc_pp_tok_s **tt, bool mode_pp) {
}

void ccp_constant_expression(cc_pp_tok_s **tt, bool mode_pp) {
		
}
