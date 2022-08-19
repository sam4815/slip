#include "lval_operations.h"
#include "assert.h"
#include "database.h"

lval* select(lenv* e, lval* v) {
  ASSERT_CHILD_TYPE(v, LVAL_STR, 0, "select");
  // ASSERT_MIN_NUM_ARGS(v, 2, "select");

  lval* field = pop_lval(v, 0);

  return field;
}

lval* insert(lenv* e, lval* v) {
  ASSERT_CHILD_TYPE(v, LVAL_STR, 0, "insert");
  // ASSERT_MIN_NUM_ARGS(v, 2, "insert");

  lval* field = pop_lval(v, 0);

  return field;
}

lval* delete(lenv* e, lval* v) {
  ASSERT_CHILD_TYPE(v, LVAL_STR, 0, "delete");
  // ASSERT_MIN_NUM_ARGS(v, 2, "delete");

  lval* field = pop_lval(v, 0);

  return field;
}
