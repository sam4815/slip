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
	set_func(e, "len", len);
	set_func(e, "init", init);

	set_func(e, "+", add);
	set_func(e, "-", subtract);
	set_func(e, "*", multiply);
	set_func(e, "/", divide);
	set_func(e, "%", modulo);
	set_func(e, "^", power);
	set_func(e, "min", minimum);
	set_func(e, "max", maximum);
}

lval* evaluate_lval(lenv* e, lval* v) {
	// Return corresponding variable
	if (v->type == LVAL_SYM) {
		lval* x = get_lval(e, v);
		delete_lval(v);
		return x;
	}

  if (v->type == LVAL_QEXPR) { return v; }
	if (v->count == 0) { return v; }
	if (v->count == 1) { return extract_lval(v, 0); }

	// Iterate over s-expression and evaluate all children
	for (int i = 0; i < v->count; i++) {
		v->cell[i] = evaluate_lval(e, v->cell[i]);
		if (v->cell[i]->type == LVAL_ERR) { return extract_lval(v, i); }
	}

	// Assume v is an s-expression and perform the first child (a function) on the remaining children
	LASSERT(v, v->cell[0]->type == LVAL_FUNC, "S-expression does not start with a function");
	lval* func = pop_lval(v, 0);

	lval* result = func->func(e, v);
	delete_lval(func);

	return result;
};

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

	while (1) {
		char* input = readline("slip> ");
		add_history(input);

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

	mpc_cleanup(6, Slip, Sexpression, Qexpression, Expression, Symbol, Number);
	
	return 0;
}
