#include "environment.h"
#include "comparators.h"
#include "conditionals.h"
#include "numbers.h"
#include "qexpressions.h"
#include "lambdas.h"
#include "load_file.h"
#include "logical_operators.h"
#include "library.h"
#include "slip.h"

void set_built_in_functions(lenv* e) {
  // Q-expression operations
	set_func_on_env(e, "list", list);
	set_func_on_env(e, "join", join);
	set_func_on_env(e, "head", head);
	set_func_on_env(e, "tail", tail);
	set_func_on_env(e, "eval", eval);
	set_func_on_env(e, "cons", cons);
	set_func_on_env(e, "init", init);
  set_func_on_env(e, "def", global_def);
  set_func_on_env(e, "=", local_def);
  set_func_on_env(e, "\\", lambda);

  // Number operations
	set_func_on_env(e, "+", add);
	set_func_on_env(e, "-", subtract);
	set_func_on_env(e, "*", multiply);
	set_func_on_env(e, "/", divide);
	set_func_on_env(e, "%", modulo);
	set_func_on_env(e, "^", power);
	set_func_on_env(e, "min", minimum);
	set_func_on_env(e, "max", maximum);

  // Comparators
  set_func_on_env(e, "==", equal);
  set_func_on_env(e, "!=", not_equal);
	set_func_on_env(e, ">", greater_than);
	set_func_on_env(e, ">=", greater_than_or_equal);
	set_func_on_env(e, "<", less_than);
	set_func_on_env(e, "<=", less_than_or_equal);

  // Logical operators
  set_func_on_env(e, "and", and);
  set_func_on_env(e, "or", or);
  set_func_on_env(e, "not", not);

  // Conditionals
  set_func_on_env(e, "if", if_condition);

  // Load .slip files
  set_func_on_env(e, "load_file", load_file);
}

void build_library(lenv* e) {
  set_built_in_functions(e);
  evaluate_string(e, "load_file \"build/library.slip\"");
}
