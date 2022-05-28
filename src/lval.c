#include <stdio.h>
#include "lval.h"

lval lval_num(long x) {
	lval v;
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

lval lval_err(int x) {
	lval v;
	v.type = LVAL_ERR;
	v.err = x;
	return v;
}

void print_lval(lval v) {
	if (v.type == LVAL_NUM) {
		printf("%li\n", v.num);
		return;
	}

	switch (v.err) {
		case LERR_DIV_ZERO:
			printf("error: division by zero.\n");
			break;
		case LERR_BAD_OP:
			printf("error: invalid operator.\n");
			break;
		case LERR_BAD_NUM:
			printf("error: invalud number.\n");
			break;
	}
}


