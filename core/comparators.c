#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lval_definitions.h"
#include "lval_operations.h"
#include "assert.h"
#include "stringify.h"
#include "environment.h"
#include "comparators.h"

bool are_lvals_equal(lval* a, lval* b) {
  if (a->type != b->type) return false;

  if (a->type == LVAL_NUM) { return a->num == b->num; }
  if (a->type == LVAL_SYM) { return strcmp(a->sym, b->sym) == 0; }
  if (a->type == LVAL_ERR) { return strcmp(a->err, b->err) == 0; }

  if (a->type == LVAL_QEXPR || a->type == LVAL_SEXPR) {
    if (a->count != b->count) return false;

    for (int i = 0; i < a->count; i++) {
      if (!are_lvals_equal(a->cell[i], b->cell[i])) return false;
    }

    return true;
  }

  if (a->type == LVAL_FUNC && a->func != NULL) { return a->func == b->func; }

  if (a->type == LVAL_FUNC && a->func == NULL) {
    if (!are_lvals_equal(a->arguments, b->arguments)) return false;
    if (!are_lvals_equal(a->body, b->body)) return false;
    return true;
  }

  return false;
}

lval* equal(lenv* e, lval* v) {
  ASSERT_MIN_NUM_ARGS(v, 2, "equal");

  bool is_equal = false;

  lval* first_child = pop_lval(v, 0);

  while (v->count != 0) {
    lval* next_child = pop_lval(v, 0);
    is_equal = are_lvals_equal(first_child, next_child);
    delete_lval(next_child);
  }

  delete_lvals(2, first_child, v);

  return is_equal ? lval_bool(1) : lval_bool(0);
}

lval* not_equal(lenv* e, lval* v) {
  lval* result = equal(e, v);
  result->boole = !result->boole;
  return result;
}

lval* compare_numbers(lenv* e, lval* v, char* operation) {
  ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, operation);
  ASSERT_MIN_NUM_ARGS(v, 2, operation);

  bool accumulator = false;
  lval* first_child = pop_lval(v, 0);
  int x = first_child->num;

  while (v->count != 0) {
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, operation);
    lval* next_child = pop_lval(v, 0);
    int y = next_child->num;

    if (strcmp(operation, ">") == 0) { accumulator = x > y; }
    if (strcmp(operation, "<") == 0) { accumulator = x < y; }
    if (strcmp(operation, ">=") == 0) { accumulator = x >= y; }
    if (strcmp(operation, "<=") == 0) { accumulator = x <= y; }

    delete_lval(next_child);
  }

  delete_lvals(2, first_child, v);

  return accumulator ? lval_bool(1) : lval_bool(0);
}

lval* greater_than(lenv* e, lval* v) { return compare_numbers(e, v, ">"); }
lval* greater_than_or_equal(lenv* e, lval* v) { return compare_numbers(e, v, ">="); }
lval* less_than(lenv* e, lval* v) { return compare_numbers(e, v, "<"); }
lval* less_than_or_equal(lenv* e, lval* v) { return compare_numbers(e, v, "<="); }
