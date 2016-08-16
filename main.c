#include "pp.h"

#define TC_ROOT "testcases/"

#define TC_PP1 TC_ROOT "pp/tc1"
#define TC_PP2 TC_ROOT "pp/tc2"
#define TC_PP3 TC_ROOT "pp/tc3"

int main(int argc, char *argv[]) {
	cc_buf_s src = cc_read_file(TC_PP3);
	
	init_log(stdout);
	if(src.bsize) {
		cc_buf_s results = cc_pp_parse(src);
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}

