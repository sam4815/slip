#include <string.h>
#include "lval_operations.h"
#include "assert.h"
#include "stringify.h"
#include "qexpressions.h"
#include "environment.h"
#include "lambdas.h"

lval* lambda(lenv* e, lval* v) {
  ASSERT_NUM_ARGS(v, 2, "lambda");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "lambda");
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 1, "lambda");

  lval* arguments = pop_lval(v, 0);
  lval* body = pop_lval(v, 0);

  delete_lval(v);

  return lval_lambda(arguments, body);
}

lval* call_lambda(lenv* env, lval* func, lval* arguments) {
  if (func->func) { return func->func(env, arguments); }

  int num_args_given = arguments->count;
  int num_args_expected = func->arguments->count;

  while (arguments->count) {
    ASSERT(arguments, func->arguments->count > 0,
      "Function passed too many arguments. Got %i, expected %i.", num_args_given, num_args_expected);

    lval* sym = pop_lval(func->arguments, 0);

    // Special case to deal with symbol for variable number of arguments ("&")
    if (strcmp(sym->sym, "&") == 0) {
      if (func->arguments->count != 1) {
        delete_lvals(2, arguments, sym);
        return lval_err("Invalid function format. Must follow & with at least one symbol.");
      }

      lval* remaining_args_sym = pop_lval(func->arguments, 0);
      set_lval_on_env(func->env, remaining_args_sym, list(env, arguments));
      delete_lvals(2, sym, remaining_args_sym);
      break;
    }

    lval* val = pop_lval(arguments, 0);

    set_lval_on_env(func->env, sym, val);
    delete_lvals(2, sym, val);
  }

  delete_lval(arguments);

  // If & remains in list of expected function arguments, bind to empty list
  if (func->arguments->count > 0 && strstr(func->arguments->cell[0]->sym, "&")) {
    if (func->arguments->count != 2) {
      return lval_err("Invalid function format. Must follow & with at least one symbol.");
    }

    delete_lval(pop_lval(func->arguments, 0));

    lval* sym = pop_lval(func->arguments, 0);
    lval* val = lval_sexpr();

    set_lval_on_env(func->env, sym, val);
    delete_lvals(2, sym, val);
  }

  // If there are remaining arguments to be bound, return partial
  if (func->arguments->count > 0) { return copy_lval(func); }

  // Else set function's parent environment to current environment,
  // append a copy of the body to a sexpr, then evaluate it
  func->env->parent = env;
  return eval(func->env, append_lval(lval_sexpr(), copy_lval(func->body)));
}
