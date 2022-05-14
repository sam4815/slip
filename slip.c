#include <stdio.h>
#include <stdlib.h>
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
 
int main(int argc, char** argv) {
	mpc_parser_t* Slip = mpc_new("slip");
	mpc_parser_t* Expression = mpc_new("expression");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Number = mpc_new("number");

	mpca_lang(MPCA_LANG_DEFAULT,
		"									\
			operator   : '+' | '-' | '*' | '/' ;				\
			number     : /-?[0-9]+/ ;					\
			expression : <number> | '(' <operator> <expression>+ ')' ;	\
			slip	   : /^/ <operator> <expression>+ /$/ ;	  		\
		",
	Slip, Expression, Operator, Number);
	puts("slip v0.1\n");

	while (1) {
		char* input = readline("slip> ");
		add_history(input);

		mpc_result_t r;

		if (mpc_parse("<stdin>", input, Slip, &r)) {
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		} else {
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		
		printf("No u a %s\n", input);
		free(input);
	}

	mpc_cleanup(4, Slip, Expression, Operator, Number);
	return 0;
}

