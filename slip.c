#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "mpc/mpc.h"

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

long operate(char* operation, long x, long y) {
	if (strcmp(operation, "+") == 0) { return x + y; }
	if (strcmp(operation, "-") == 0) { return x - y; }
	if (strcmp(operation, "/") == 0) { return x / y; }
	if (strcmp(operation, "*") == 0) { return x * y; }
	if (strcmp(operation, "%") == 0) { return x % y; }
	if (strcmp(operation, "^") == 0) { return pow(x, y); }

	if (strcmp(operation, "max") == 0) { return x > y ? x : y; } 
	if (strcmp(operation, "min") == 0) { return x < y ? x : y; } 

	return 0;
}

long parse_tree(mpc_ast_t* tree) {
	bool is_number = strstr(tree->tag, "number") != NULL;	
	if (is_number) {
		return(atoi(tree->contents));
	}

	long accumulator = parse_tree(tree->children[2]);

	for (int i = 3; i < tree->children_num - 1; i++) {
		long child_value = parse_tree(tree->children[i]);
		accumulator = operate(tree->children[1]->contents, accumulator, child_value);
	}

	return accumulator;
}

int main(int argc, char** argv) {
	mpc_parser_t* Slip = mpc_new("slip");
	mpc_parser_t* Expression = mpc_new("expression");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Number = mpc_new("number");

	mpca_lang(MPCA_LANG_DEFAULT,
			"                                                                       \
			operator   : '+' | '-' | '*' | '/' | '%' | '^'                          \
			           | \"min\" | \"max\" ;                                        \
			number     : /-?[0-9]+/ ;                                               \
			expression : <number> | '(' <operator> <expression>+ ')' ;              \
			slip	   : /^/ <operator> <expression>+ /$/ ;                         \
			",
			Slip, Expression, Operator, Number);

	puts("slip v0.1\n");

	while (1) {
		char* input = readline("slip> ");
		add_history(input);

		mpc_result_t result;
		mpc_parse("<stdin>", input, Slip, &result);

		if (getenv("PRINT_TREE")) { mpc_ast_print(result.output); }

		long output = parse_tree(result.output);
		printf("%ld\n", output);

		free(input);
		mpc_ast_delete(result.output);
	}

	mpc_cleanup(4, Slip, Expression, Operator, Number);
	
	return 0;
}

