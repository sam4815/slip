#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lval_operations.h"
#include "assert.h"
#include "numbers.h"

lval* operate(lenv* e, lval* v, char* operation) {
  ASSERT_MIN_NUM_ARGS(v, 1, operation);
  ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, operation);
	lval* accumulator = pop_lval(v, 0);

	// If operation is subtract, and there are no
	// further arguments, negate it
	if (strcmp(operation, "-") == 0 && v->count == 0) {
		accumulator->num = -(accumulator->num);
	}

	while (v->count != 0) {
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, operation);
		lval* next_lval = pop_lval(v, 0);

		int x = accumulator->num;
		int y = next_lval->num;

		// Catch division by zero error
		if (strcmp(operation, "/") == 0 && y == 0) {
			delete_lvals(2, accumulator, next_lval);
			accumulator = lval_err("Can't divide by 0.");
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
