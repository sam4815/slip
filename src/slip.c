#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "../mpc/mpc.h"
#include "lval.h"

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

lval operate(char* operation, lval x, lval y) {
	// If either value is an error, just return it
	if (x.type == LVAL_ERR) { return x; }
	if (y.type == LVAL_ERR) { return y; }

	// Catch division by zero error
	if (strcmp(operation, "/") == 0 && y.num == 0) { return lval_err(LERR_DIV_ZERO); }

	if (strcmp(operation, "+") == 0) { return lval_num(x.num + y.num); }
	if (strcmp(operation, "-") == 0) { return lval_num(x.num - y.num); }
	if (strcmp(operation, "/") == 0) { return lval_num(x.num / y.num); }
	if (strcmp(operation, "*") == 0) { return lval_num(x.num * y.num); }
	if (strcmp(operation, "%") == 0) { return lval_num(x.num % y.num); }
	if (strcmp(operation, "^") == 0) { return lval_num(pow(x.num, y.num)); }

	if (strcmp(operation, "max") == 0) { return x.num > y.num ? x : y; } 
	if (strcmp(operation, "min") == 0) { return x.num < y.num ? x : y; } 

	return lval_err(LERR_BAD_OP);
}

lval evaluate_tree(mpc_ast_t* tree) {
	bool is_number = strstr(tree->tag, "number") != NULL;	
	if (is_number) {
		errno = 0;
		long number = strtol(tree->contents, NULL, 10);
		return errno == ERANGE ? lval_err(LERR_BAD_NUM) : lval_num(number);
	}

	lval accumulator = evaluate_tree(tree->children[2]);

	for (int i = 3; i < tree->children_num - 1; i++) {
		lval child_value = evaluate_tree(tree->children[i]);
		accumulator = operate(tree->children[1]->contents, accumulator, child_value);
	}

	return accumulator;
}

int main(int argc, char** argv) {
	mpc_parser_t* Slip = mpc_new("slip");
	mpc_parser_t* Expression = mpc_new("expr");
	mpc_parser_t* Sexpression = mpc_new("sexpr");
	mpc_parser_t* Symbol = mpc_new("symbol");
	mpc_parser_t* Number = mpc_new("number");

	mpca_lang(MPCA_LANG_DEFAULT,
			"                                                                       \
			symbol     : '+' | '-' | '*' | '/' | '%' | '^'                          \
			           | \"min\" | \"max\" ;                                        \
			number     : /-?[0-9]+/ ;                                               \
			sexpr      : '(' <expr>* ')' ;                                          \
			expr       : <number> | <sexpr> | <symbol> ;                            \
			slip	   : /^/ <expr>* /$/ ;                                          \
			",
			Slip, Sexpression, Expression, Symbol, Number);

	puts("slip v0.1\n");

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

		print_lval(evaluate_tree(result.output));

		free(input);
		mpc_ast_delete(result.output);
	}

	mpc_cleanup(5, Slip, Sexpression, Expression, Symbol, Number);
	
	return 0;
}

