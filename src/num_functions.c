#include <stdio.h>
#include "../mpc/mpc.h"
#include "lval.h"
#include "lenv.h"

lval* operate(lenv* e, lval* v, char* operation) {
	lval* accumulator = pop_lval(v, 0);

	// If operation is subtract, and there are no
	// further arguments, negate it
	if (strcmp(operation, "-") == 0 && v->count == 0) {
		accumulator->num = -(accumulator->num);
	}

	while (v->count != 0) {
		lval* next_lval = pop_lval(v, 0);

		if (accumulator->type != LVAL_NUM || next_lval->type != LVAL_NUM) {
			delete_lvals(2, accumulator, next_lval);
			accumulator = lval_err("Cannot operate on non-number");
			break;
		}

		int x = accumulator->num;
		int y = next_lval->num;

		// Catch division by zero error
		if (strcmp(operation, "/") == 0 && y == 0) {
			delete_lvals(2, accumulator, next_lval);
			accumulator = lval_err("Can't divide by 0");
			break;
		}

		if (strcmp(operation, "+") == 0) { accumulator->num = x + y; }
		if (strcmp(operation, "-") == 0) { accumulator->num = x - y; }
		if (strcmp(operation, "/") == 0) { accumulator->num = x / y; }
		if (strcmp(operation, "*") == 0) { accumulator->num = x * y; }
		if (strcmp(operation, "%") == 0) { accumulator->num = x % y; }
		if (strcmp(operation, "^") == 0) { accumulator->num = pow(x, y); }
		if (strcmp(operation, "max") == 0) { accumulator->num = x > y ? x : y; }
		if (strcmp(operation, "min") == 0) { accumulator->num = x < y ? x : y; }
		
		delete_lval(next_lval);
	}
	
	delete_lval(v);
	return accumulator;
}

lval* add(lenv* e, lval* a) { return operate(e, a, "+"); }
lval* subtract(lenv* e, lval* a) { return operate(e, a, "-"); }
lval* multiply(lenv* e, lval* a) { return operate(e, a, "*"); }
lval* divide(lenv* e, lval* a) { return operate(e, a, "/"); }
lval* modulo(lenv* e, lval* a) { return operate(e, a, "%"); }
lval* power(lenv* e, lval* a) { return operate(e, a, "^"); }
lval* maximum(lenv* e, lval* a) { return operate(e, a, "max"); }
lval* minimum(lenv* e, lval* a) { return operate(e, a, "min"); }
