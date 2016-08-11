#include "pp.h"
#include <stdio.h>

#define TC_ROOT "testcases/"

#define TC_PP1 TC_ROOT "pp/tc1"
#define TC_PP2 TC_ROOT "pp/tc2"

int main(int argc, char *argv[]) {
	cc_buf_s src = cc_read_file(TC_PP2);
	if(src.bsize) {
		cc_buf_s results = cc_pp_parse(src);
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}

