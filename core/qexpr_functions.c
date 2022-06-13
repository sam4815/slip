#include <stdio.h>
#include "mpc/mpc.h"
#include "lval.h"
#include "lenv.h"

lval* global_def(lenv* e, lval* v) {
	ASSERT_VALID_DEF(v, "def");

  lval* syms = v->cell[0];
  for (int i = 0; i < syms->count; i++) {
    ASSERT_CHILD_TYPE(syms, LVAL_SYM, i, "def");
    set_global_lval(e, syms->cell[i], v->cell[i + 1]);
  }

  delete_lval(v);
  return lval_sexpr();
}

lval* local_def(lenv* e, lval* v) {
	ASSERT_VALID_DEF(v, "=");

  lval* syms = v->cell[0];
  for (int i = 0; i < syms->count; i++) {
    ASSERT_CHILD_TYPE(syms, LVAL_SYM, i, "=");
    set_lval(e, syms->cell[i], v->cell[i + 1]);
  }

  delete_lval(v);
  return lval_sexpr();
}

lval* lambda(lenv* e, lval* v) {
  ASSERT_NUM_ARGS(v, 2, "lambda");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "lambda");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 1, "lambda");

  lval* arguments = pop_lval(v, 0);
  lval* body = pop_lval(v, 0);

  delete_lval(v);

  return lval_lambda(arguments, body);
}

lval* head(lenv* e, lval* v) {
	ASSERT_VALID_QEXPR_ARG("head", v);

	lval* child = extract_lval(v, 0);
	while (child->count > 1) {
		delete_lval(pop_lval(child, 1));
	}
	return child;
}

lval* tail(lenv* e, lval* v) {
	ASSERT_VALID_QEXPR_ARG("tail", v);

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
	ASSERT_NUM_ARGS(v, 1, "eval");
	ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "eval");

	lval* child = extract_lval(v, 0);
	child->type = LVAL_SEXPR;
	return evaluate_lval(e, child);
}

lval* join(lenv* e, lval* v) {
	for (int i = 0; i < v->count; i++) {
    ASSERT_CHILD_TYPE(v, LVAL_QEXPR, i, "join");
	}

	lval* accumulator = pop_lval(v, 0);

	while (v->count > 0) {
		join_lval(accumulator, pop_lval(v, 0));
	};

	delete_lval(v);

	return accumulator;
}

lval* cons(lenv* e, lval* v) {
  ASSERT_NUM_ARGS(v, 2, "cons");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 1, "cons");
	
	lval* value_to_append = pop_lval(v, 0);
	lval* qexpr = pop_lval(v, 0);

	append_lval(qexpr, value_to_append);
	delete_lval(v);

	return qexpr;
}

lval* len(lenv* e, lval* v) {
  ASSERT_NUM_ARGS(v, 1, "len");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "len");

	lval* length = lval_num(v->cell[0]->count);
	delete_lval(v);

	return length;
}

lval* init(lenv* e, lval* v) {
  ASSERT_NUM_ARGS(v, 1, "init");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "init");
	
	lval* child = extract_lval(v, 0);
	delete_lval(pop_lval(child, child->count - 1));

	return child;
}
