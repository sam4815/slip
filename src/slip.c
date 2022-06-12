#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "../mpc/mpc.h"
#include "lval.h"
#include "lenv.h"
#include "qexpr_functions.h"
#include "num_functions.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(copy) - 1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

void set_built_in_functions(lenv* e) {
	set_func(e, "list", list);
	set_func(e, "join", join);
	set_func(e, "head", head);
	set_func(e, "tail", tail);
	set_func(e, "eval", eval);
	set_func(e, "cons", cons);
	set_func(e, "init", init);
	set_func(e, "len", len);
  set_func(e, "def", global_def);
  set_func(e, "=", local_def);
  set_func(e, "\\", lambda);

	set_func(e, "+", add);
	set_func(e, "-", subtract);
	set_func(e, "*", multiply);
	set_func(e, "/", divide);
	set_func(e, "%", modulo);
	set_func(e, "^", power);
	set_func(e, "min", minimum);
	set_func(e, "max", maximum);

  set_func(e, "env", print_env);
}

lval* call_lval(lenv* env, lval* func, lval* arguments) {
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
      set_lval(func->env, remaining_args_sym, list(env, arguments));
      delete_lvals(2, sym, remaining_args_sym);
      break;
    }

    lval* val = pop_lval(arguments, 0);

    set_lval(func->env, sym, val);
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

    set_lval(func->env, sym, val);
    delete_lvals(2, sym, val);
  }

  // If there are remaining arguments to be bound, return partial
  if (func->arguments->count > 0) { return copy_lval(func); }

  // Else set function's parent environment to current environment,
  // append a copy of the body to a sexpr, then evaluate it
  func->env->parent = env;
  return eval(func->env, append_lval(lval_sexpr(), copy_lval(func->body)));
}

lval* evaluate_lval(lenv* env, lval* val) {
	// Return corresponding variable
	if (val->type == LVAL_SYM) {
		lval* env_val = get_lval(env, val);
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

void execute_code(mpc_parser_t* Slip, lenv* e, char* code) {
  mpc_result_t result;
	mpc_parse("input", code, Slip, &result);
  evaluate_lval(e, parse_lval(result.output));
}

void build_library(mpc_parser_t* Slip, lenv* e) {
  execute_code(Slip, e,
    "def {fun} (\\ {args body} {def (head args) (\\ (tail args) body)})");
}

int main(int argc, char** argv) {
	mpc_parser_t* Slip = mpc_new("slip");
	mpc_parser_t* Expression = mpc_new("expr");
	mpc_parser_t* Sexpression = mpc_new("sexpr");
	mpc_parser_t* Qexpression = mpc_new("qexpr");
	mpc_parser_t* Symbol = mpc_new("symbol");
	mpc_parser_t* Number = mpc_new("number");

	mpca_lang(MPCA_LANG_DEFAULT,
			"                                                                       \
			symbol     : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;                         \
			number     : /-?[0-9]+/ ;                                               \
			sexpr      : '(' <expr>* ')' ;                                          \
			qexpr      : '{' <expr>* '}' ;                                          \
			expr       : <number> | <sexpr> | <qexpr> | <symbol> ;                  \
			slip	   : /^/ <expr>* /$/ ;                                          \
			",
			Slip, Sexpression, Qexpression, Expression, Symbol, Number);

	puts("slip v0.1\n");

	lenv* environment = initialize_env();
	set_built_in_functions(environment);
  build_library(Slip, environment);

	while (1) {
		char* input = readline("🍂 ");
		if (strlen(input) > 0) { add_history(input); }

		mpc_result_t result;
		bool success = mpc_parse("<stdin>", input, Slip, &result);

		if (!success) {
			mpc_err_print(result.error);
			free(input);
			continue;
		}

		if (getenv("PRINT_TREE")) { mpc_ast_print(result.output); }
		
		lval* evaluation = evaluate_lval(environment, parse_lval(result.output));
		println_lval(evaluation);

		free(input);
		delete_lval(evaluation);
		mpc_ast_delete(result.output);
	}

  delete_env(environment);
	mpc_cleanup(6, Slip, Sexpression, Qexpression, Expression, Symbol, Number);
	
	return 0;
}
