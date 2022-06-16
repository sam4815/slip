#include <stdio.h>
#include "lval_definitions.h"
#include "lval_operations.h"
#include "qexpressions.h"
#include "assert.h"
#include "environment.h"
#include "conditionals.h"

lval* if_condition(lenv* e, lval* v) {
  ASSERT_CHILD_TYPE(v, LVAL_BOOL, 0, "if");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 1, "if");
  ASSERT_MIN_NUM_ARGS(v, 2, "if");
  ASSERT_MAX_NUM_ARGS(v, 3, "if");

  lval* condition = pop_lval(v, 0);
  lval* result = lval_sexpr();

  if (condition->boole) {
    result = eval(e, append_lval(lval_sexpr(), pop_lval(v, 0)));
  } else if (v->count > 1) {
    result = eval(e, append_lval(lval_sexpr(), pop_lval(v, 1)));
  }

  delete_lvals(2, condition, v);

  return result;
}
