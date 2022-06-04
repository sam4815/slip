#include <stdio.h>
#include "../mpc/mpc.h"
#include "lval.h"
#include "lenv.h"

lval* head(lenv* e, lval* v) {
	LASSERT(v, v->count == 1, "Function 'head' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'head' passed incorrect type");
	LASSERT(v, v->cell[0]->count != 0, "Function 'head' passed empty list");

	lval* child = extract_lval(v, 0);
	while (child->count > 1) {
		delete_lval(pop_lval(child, 1));
	}
	return child;
}

lval* tail(lenv* e, lval* v) {
	LASSERT(v, v->count == 1, "Function 'tail' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'tail' passed incorrect type");
	LASSERT(v, v->cell[0]->count != 0, "Function 'tail' passed empty list");

	lval* child = extract_lval(v, 0);
	delete_lval(pop_lval(child, 0));
	return child;
}

lval* list(lenv* e, lval* v) {
	v->type = LVAL_QEXPR;
	return v;
}

lval* evaluate_lval(lenv* e, lval* v);
lval* eval(lenv* e, lval* v) {
	printf("%c", v->cell[0]->type);

	LASSERT(v, v->count == 1, "Function 'eval' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed incorrect type");

	lval* child = extract_lval(v, 0);
	child->type = LVAL_SEXPR;
	return evaluate_lval(e, child);
}

lval* join(lenv* e, lval* v) {
	for (int i = 0; i < v->count; i++) {
		LASSERT(v, v->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type");
	}

	lval* accumulator = pop_lval(v, 0);

	while (v->count > 0) {
		join_lval(accumulator, pop_lval(v, 0));
	};

	delete_lval(v);

	return accumulator;
}

lval* cons(lenv* e, lval* v) {
	LASSERT(v, v->count == 2, "Function 'cons' passed wrong number of arguments");
	LASSERT(v, v->cell[1]->type == LVAL_QEXPR, "Function 'cons' passed incorrect type");
	
	lval* value_to_append = pop_lval(v, 0);
	lval* qexpr = pop_lval(v, 0);

	append_lval(qexpr, value_to_append);
	delete_lval(v);

	return qexpr;
}

lval* len(lenv* e, lval* v) {
	LASSERT(v, v->count == 1, "Function 'len' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'len' passed incorrect type");

	lval* length = lval_num(v->cell[0]->count);
	delete_lval(v);

	return length;
}

lval* init(lenv* e, lval* v) {
	LASSERT(v, v->count == 1, "Function 'len' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'len' passed incorrect type");
	
	lval* child = extract_lval(v, 0);
	delete_lval(pop_lval(child, child->count - 1));

	return child;
}
