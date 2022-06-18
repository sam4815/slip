#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lval_operations.h"
#include "lval_parser.h"
#include "lval_stringify.h"
#include "environment.h"
#include "lambdas.h"
#include "library.h"
#include "slip.h"

lval* call_lval(lenv* env, lval* func, lval* arguments) {
  if (func->func) { return func->func(env, arguments); }

  return call_lambda(env, func, arguments);
}

lval* evaluate_lval(lenv* env, lval* val) {
	// Return corresponding variable
	if (val->type == LVAL_SYM) {
		lval* env_val = get_lval_from_env(env, val);
		delete_lval(val);
		return env_val;
	}

  if (val->type != LVAL_SEXPR || val->count == 0) { return val; }

	// Evaluate all children
	for (int i = 0; i < val->count; i++) {
		val->cell[i] = evaluate_lval(env, val->cell[i]);
		if (val->cell[i]->type == LVAL_ERR) { return extract_lval(val, i); }
	}

  // If the first child isn't a function, just extract and return the final evaluated child
  if (val->cell[0]->type != LVAL_FUNC) { return extract_lval(val, val->count - 1); }

	// Assume val is an s-expression and perform the first child (a function) on the remaining children
	lval* func = pop_lval(val, 0);
	lval* result = call_lval(env, func, val);

	delete_lval(func);
	return result;
};

void destroy_slip(mpc_parser_t* Parser, lenv* e) {
  delete_env(e);
	mpc_cleanup(9, Parser, Expression, Sexpression, Qexpression,
    Symbol, String, Comment, Boolean, Number);
}

char* evaluate_string(mpc_parser_t* Parser, lenv* e, char* input) {
  mpc_result_t result;
  int parse_success = mpc_parse("input", input, Parser, &result);

	if (!parse_success) { return "Error parsing input"; }

  lval* evaluation = evaluate_lval(e, parse_lval(result.output));
  char* evaluation_str = stringify_lval(evaluation);

  delete_lval(evaluation);
  mpc_ast_delete(result.output);

  return evaluation_str;
}

slip* initialize_slip(void) {
  slip* slip_ptr = malloc(sizeof(slip));

	Parser = mpc_new("slip");
	Expression = mpc_new("expr");
	Sexpression = mpc_new("sexpr");
	Qexpression = mpc_new("qexpr");
	Symbol = mpc_new("symbol");
	String = mpc_new("string");
	Comment = mpc_new("comment");
	Boolean = mpc_new("boolean");
	Number = mpc_new("number");

	mpca_lang(MPCA_LANG_DEFAULT,
			"                                                                       \
			symbol     : /[a-zA-Z0-9_+\\^\\-*\\/\\\\=<>!&]+/ ;                      \
			number     : /-?[0-9]+/ ;                                               \
      string     : /\"(\\\\.|[^\"])*\"/ ;                                     \
      boolean    : /true|false/ ;                                             \
      comment    : /;[^\\r\\n]*/ ;                                            \
			sexpr      : '(' <expr>* ')' ;                                          \
			qexpr      : '{' <expr>* '}' ;                                          \
			expr       : <number> | <string> | <boolean> | <comment>                \
                 | <sexpr> | <qexpr> | <symbol> ;                             \
			slip	     : /^/ <expr>* /$/ ;                                          \
			",
			Parser, Sexpression, Qexpression, Expression,
      Symbol, String, Comment, Boolean, Number);

	lenv* environment = initialize_env();
  build_library(Parser, environment);

  slip_ptr->parser = Parser;
  slip_ptr->environment = environment;
  slip_ptr->evaluate_string = evaluate_string;
  slip_ptr->destroy = destroy_slip;
	
	return slip_ptr;
}
