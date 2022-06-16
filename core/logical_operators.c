#include "lval_operations.h"
#include "assert.h"
#include "logical_operators.h"

lval* true_lval(lenv* e, lval* v) {
  return lval_bool(1);
}

lval* false_lval(lenv* e, lval* v) {
  return lval_bool(0);
}

lval* and(lenv* e, lval* v) {
  ASSERT_MIN_NUM_ARGS(v, 2, "and");

  while (v->count != 0) {
    ASSERT_CHILD_TYPE(v, LVAL_BOOL, 0, "and");
    lval* next_child = pop_lval(v, 0);

    if (!next_child->boole) {
      delete_lvals(2, v, next_child);
      return lval_bool(0);
    }
  }

  delete_lval(v);
  return lval_bool(1);
}

lval* or(lenv* e, lval* v) {
  ASSERT_MIN_NUM_ARGS(v, 2, "or");

  while (v->count != 0) {
    ASSERT_CHILD_TYPE(v, LVAL_BOOL, 0, "or");
    lval* next_child = pop_lval(v, 0);

    if (next_child->boole) {
      delete_lvals(2, v, next_child);
      return lval_bool(1);
    }
  }

  delete_lval(v);
  return lval_bool(0);
}

lval* not(lenv* e, lval* v) {
  ASSERT_CHILD_TYPE(v, LVAL_BOOL, 0, "not");
  ASSERT_NUM_ARGS(v, 1, "not");

  lval* child = pop_lval(v, 0);
  child->boole = !child->boole;

  delete_lval(v);
  return child;
}
