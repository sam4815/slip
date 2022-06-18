#include "lval_definitions.h"
#include "lval_stringify.h"

#define ASSERT(val, cond, fmt, ...) \
	if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); delete_lval(val); return err; }

#define ASSERT_NUM_ARGS(val, num, name) \
  ASSERT(val, val->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, expected %i.", name, val->count, num);

#define ASSERT_MIN_NUM_ARGS(val, num, name) \
  ASSERT(val, val->count >= num, \
    "Function '%s' passed incorrect number of arguments. Got %i, expected at least %i.", name, val->count, num);

#define ASSERT_MAX_NUM_ARGS(val, num, name) \
  ASSERT(val, val->count <= num, \
    "Function '%s' passed incorrect number of arguments. Got %i, expected at most %i.", name, val->count, num);

#define ASSERT_TYPE(val, target, name) \
  ASSERT(val, val->type == target, \
    "Function '%s' passed incorrect type. Got %s, expected %s.", name, stringify_type(val->type), stringify_type(target));

#define ASSERT_CHILD_NOT_EMPTY(val, name) \
  ASSERT(val, val->cell[0]->count != 0, "Function '%s' passed empty list.", name);

#define ASSERT_CHILD_TYPE(val, target, index, name) \
  ASSERT(val, val->cell[index]->type == target, \
    "Function '%s' passed incorrect type. Got %s, expected %s.", name, stringify_type(val->cell[index]->type), stringify_type(target));

#define ASSERT_VALID_QEXPR_ARG(name, val) \
  ASSERT_NUM_ARGS(val, 1, name); \
  ASSERT_CHILD_TYPE(val, LVAL_QEXPR, 0, name); \
  ASSERT_CHILD_NOT_EMPTY(val, name);

#define ASSERT_VALID_DEF(name, val) \
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "def"); \
  ASSERT_CHILD_NOT_EMPTY(v, "def"); \
  ASSERT_NUM_ARGS(v, v->cell[0]->count + 1, "def");
