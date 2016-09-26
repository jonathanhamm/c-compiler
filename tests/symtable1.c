#include "symtable1.h"
#include <stdio.h>

static void parse_file(FILE *F);
static void print_rec(cc_sym_rec_s *rec);

int main(int argc, char *argv[]) {	
	if(argc > 1) {
		int i;

		for(i = 1; i < argc; i++) {
			FILE *f = fopen(argv[i], "r");
			if(f) {
				parse_file(f);
				fclose(f);
			}
			else {
				fprintf(stderr, "Error Reading file: %s\n", argv[i]);
			}
		}
	}
	else {
		parse_file(stdin);
	}

	return 0;
}

void parse_file(FILE *f) {
	int c;
	cc_buf_s keybuf, valbuf, *curr = &keybuf;
	char *key, *val;
	bool wstransition = false;
	cc_sym_s table;

	cc_sym_init(&table);
	cc_buf_init(&keybuf);
	cc_buf_init(&valbuf);

	while((c = fgetc(f)) != EOF) {
		switch(c) {
			case ' ':
			case '\t':
			case '\v':
			case '\n':
			case '\r':
				if(wstransition) {
					if(valbuf.size) {
						cc_buf_str_addc(&valbuf, '\0');
						val = dupstr(valbuf.buf);
						cc_buf_clear(&valbuf);
						curr = &keybuf;
						cc_sym_insert(&table, key, val);
					}
					else if(keybuf.size) {
						cc_buf_str_addc(&keybuf, '\0');
						key = dupstr(keybuf.buf);
						cc_buf_clear(&keybuf);
						curr = &valbuf;
						
					}
					wstransition = false;
				}
				break;
			default:
				wstransition = true;
				cc_buf_str_addc(curr, c);
				break;
		}
	}
	cc_sym_foreach(&table, print_rec);
	cc_sym_destroy(&table);
	cc_buf_destroy(&keybuf);
	cc_buf_destroy(&valbuf);
}


void print_rec(cc_sym_rec_s *rec) {
	printf("keyval: %s -> %s\n", rec->key, rec->val);
}

