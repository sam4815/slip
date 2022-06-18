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

  // If the first child isn't a function, just extract and return the evaluated first child
  if (val->cell[0]->type != LVAL_FUNC) { return evaluate_lval(env, extract_lval(val, 0)); }

	// Assume val is an s-expression and perform the first child (a function) on the remaining children
	lval* func = pop_lval(val, 0);
	lval* result = call_lval(env, func, val);

	delete_lval(func);
	return result;
};

void destroy_slip(mpc_parser_t* Parser, lenv* e) {
  delete_env(e);
	mpc_cleanup(1, Parser);
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

	mpc_parser_t* Parser = mpc_new("slip");
	mpc_parser_t* Expression = mpc_new("expr");
	mpc_parser_t* Sexpression = mpc_new("sexpr");
	mpc_parser_t* Qexpression = mpc_new("qexpr");
	mpc_parser_t* Symbol = mpc_new("symbol");
	mpc_parser_t* String = mpc_new("string");
	mpc_parser_t* Comment = mpc_new("comment");
	mpc_parser_t* Number = mpc_new("number");

	mpca_lang(MPCA_LANG_DEFAULT,
			"                                                                       \
			symbol     : /[a-zA-Z0-9_+\\^\\-*\\/\\\\=<>!&]+/ ;                      \
			number     : /-?[0-9]+/ ;                                               \
      string     : /\"(\\\\.|[^\"])*\"/ ;                                     \
      comment    : /;[^\\r\\n]*/ ;                                            \
			sexpr      : '(' <expr>* ')' ;                                          \
			qexpr      : '{' <expr>* '}' ;                                          \
			expr       : <number> | <string> | <comment>                            \
                 | <sexpr> | <qexpr> | <symbol> ;                             \
			slip	     : /^/ <expr>* /$/ ;                                          \
			",
			Parser, Sexpression, Qexpression, Expression, Symbol, String, Comment, Number);

	lenv* environment = initialize_env();
  build_library(Parser, environment);

  slip_ptr->parser = Parser;
  slip_ptr->environment = environment;
  slip_ptr->evaluate_string = evaluate_string;
  slip_ptr->destroy = destroy_slip;
	
	return slip_ptr;
}
