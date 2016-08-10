#include "pp.h"
#include <stdio.h>

#define TC_ROOT "testcases/"

#define TC_PP1 TC_ROOT "pp/tc1"

int main(int argc, char *argv[]) {
	cc_buf_s src = cc_read_file(TC_PP1);
	if(src.bsize) {
		cc_buf_s results = cc_pp_parse(src);
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}

