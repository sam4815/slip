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

lval* operate(lval* v, char* operation) {
	lval* accumulator = pop_lval(v, 0);

	// If operation is subtract, and there are no
	// further arguments, negate it
	if (strcmp(operation, "-") == 0 && v->count == 0) {
		accumulator->num = -(accumulator->num);
	}

	while (v->count != 0) {
		lval* next_lval = pop_lval(v, 0);

		if (accumulator->type != LVAL_NUM || next_lval->type != LVAL_NUM) {
			delete_lvals(2, accumulator, next_lval);
			accumulator = lval_err("Cannot operate on non-number");
			break;
		}

		int x = accumulator->num;
		int y = next_lval->num;

		// Catch division by zero error
		if (strcmp(operation, "/") == 0 && y == 0) {
			delete_lvals(2, accumulator, next_lval);
			accumulator = lval_err("Can't divide by 0");
			break;
		}

		if (strcmp(operation, "+") == 0) { accumulator->num = x + y; }
		if (strcmp(operation, "-") == 0) { accumulator->num = x - y; }
		if (strcmp(operation, "/") == 0) { accumulator->num = x / y; }
		if (strcmp(operation, "*") == 0) { accumulator->num = x * y; }
		if (strcmp(operation, "%") == 0) { accumulator->num = x % y; }
		if (strcmp(operation, "^") == 0) { accumulator->num = pow(x, y); }
		if (strcmp(operation, "max") == 0) { accumulator->num = x > y ? x : y; }
		if (strcmp(operation, "min") == 0) { accumulator->num = x < y ? x : y; }
		
		delete_lval(next_lval);
	}
	
	delete_lval(v);
	return accumulator;
}

lval* head(lval* v) {
	LASSERT(v, v->count == 1, "Function 'head' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'head' passed incorrect type");
	LASSERT(v, v->cell[0]->count != 0, "Function 'head' passed empty list");

	lval* child = extract_lval(v, 0);
	while (child->count > 1) {
		delete_lval(pop_lval(child, 1));
	}
	return child;
}

lval* tail(lval* v) {
	LASSERT(v, v->count == 1, "Function 'tail' passed too many arguments");
	LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'tail' passed incorrect type");
	LASSERT(v, v->cell[0]->count != 0, "Function 'tail' passed empty list");

	lval* child = extract_lval(v, 0);
	delete_lval(pop_lval(child, 0));
	return child;
}

lval* list(lval* v) {
	v->type = LVAL_QEXPR;
	return v;
}

lval* evaluate_lval(lval* v);
lval* eval(lval* v) {
	printf("%c", v->cell[0]->type);

	LASSERT(v, v->count == 1, "Function 'eval' passed too many arguments");
	// LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed too many arguments");

	lval* child = extract_lval(v, 0);
	child->type = LVAL_SEXPR;
	return evaluate_lval(child);
}

lval* join(lval* v) {
	for (int i = 0; i < v->count; i++) {
		LASSERT(v, v->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type");
	}

	lval* accumulator = pop_lval(v, 0);

	while (v->count > 0) {
		join_lval(accumulator, pop_lval(v, 0));
	};

	delete_lval(v);

	return accumulator;
}

lval* perform_symbol(lval* v, char* symbol) {
	if (strstr("+/-*%minmax", symbol)) { return operate(v, symbol); }
	if (strstr("list", symbol)) { return list(v); }
	if (strstr("join", symbol)) { return join(v); }
	if (strstr("head", symbol)) { return head(v); }
	if (strstr("tail", symbol)) { return tail(v); }
	if (strstr("eval", symbol)) { return eval(v); }

	delete_lval(v);
	return lval_err("Unknown symbol");
}

lval* evaluate_lval(lval* v) {
	for (int i = 0; i < v->count; i++) {
		bool is_sexpr = v->cell[i]->type == LVAL_SEXPR;
		v->cell[i] = is_sexpr ? evaluate_lval(v->cell[i]) : v->cell[i];

		if (v->cell[i]->type == LVAL_ERR) {
			return extract_lval(v, i);
		}
	}

	if (v->count == 0) { return v; }
	if (v->count == 1) { return extract_lval(v, 0); }

	LASSERT(v, v->cell[0]->type == LVAL_SYM, "S-expression does not start with a symbol");
	lval* symLval = pop_lval(v, 0);

	lval* result = perform_symbol(v, symLval->sym);
	delete_lval(symLval);

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
			symbol     : '+' | '-' | '*' | '/' | '%' | '^' |  \"min\" | \"max\"     \
			           | \"head\" | \"tail\" | \"eval\" | \"list\" | \"join\" ;     \
			number     : /-?[0-9]+/ ;                                               \
			sexpr      : '(' <expr>* ')' ;                                          \
			qexpr      : '{' <expr>* '}' ;                                          \
			expr       : <number> | <sexpr> | <qexpr> | <symbol> ;                  \
			slip	   : /^/ <expr>* /$/ ;                                          \
			",
			Slip, Sexpression, Qexpression, Expression, Symbol, Number);

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
		
		lval* evaluation = evaluate_lval(parse_lval(result.output));
		println_lval(evaluation);

		free(input);
		delete_lval(evaluation);
		mpc_ast_delete(result.output);
	}

	mpc_cleanup(6, Slip, Sexpression, Qexpression, Expression, Symbol, Number);
	
	return 0;
}

