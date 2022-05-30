#include <stdio.h>
#include <stdarg.h>
#include "../mpc/mpc.h"
#include "lval.h"

lval* lval_num(long num) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num = num;
	return v;
}

lval* lval_err(char* err) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_ERR;
	v->err = malloc(strlen(err) + 1);
	strcpy(v->err, err);
	return v;
}

lval* lval_sym(char* sym) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SYM;
	v->sym = malloc(strlen(sym) + 1);
	strcpy(v->sym, sym);
	return v;
}

lval* lval_sexpr(void) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

lval* lval_qexpr(void) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_QEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

lval* append_lval(lval* parent, lval* child) {
	parent->count = parent->count + 1;
	parent->cell = realloc(parent->cell, sizeof(lval*) * parent->count);
	parent->cell[parent->count - 1] = child;
	return parent;
}

lval* parse_lval_num(mpc_ast_t* tree) {
	errno = 0;
	long number = strtol(tree->contents, NULL, 10);
	return errno == ERANGE ? lval_err("Invalid number") : lval_num(number);
}

lval* parse_lval(mpc_ast_t* tree) {
	lval* parsed_lval = NULL;

	if (strstr(tree->tag, "number")) { parsed_lval = parse_lval_num(tree); }
	if (strstr(tree->tag, "symbol")) { parsed_lval = lval_sym(tree->contents); }
	if (strstr(tree->tag, "qexpr")) { parsed_lval = lval_qexpr(); }
	if (strstr(tree->tag, "sexpr") || strcmp(tree->tag, ">") == 0) {
		parsed_lval = lval_sexpr();
	}

	for (int i = 0; i < tree->children_num; i++) {
		if (strcmp(tree->children[i]->contents, "(") == 0) { continue; }
		if (strcmp(tree->children[i]->contents, ")") == 0) { continue; }
		if (strcmp(tree->children[i]->contents, "}") == 0) { continue; }
		if (strcmp(tree->children[i]->contents, "{") == 0) { continue; }

		if (strcmp(tree->children[i]->tag, "regex") == 0) { continue; }

		parsed_lval = append_lval(parsed_lval, parse_lval(tree->children[i]));
	}

	return parsed_lval;
}

lval* pop_lval(lval* v, int i) {
	lval* target_lval = v->cell[i];

	// Move everything from i + 1 onwards to i
	memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - 1 - i));

	v->count = v->count - 1;
	v->cell = realloc(v->cell, sizeof(lval*) * v->count);

	return target_lval;
}

lval* extract_lval(lval* v, int i) {
	lval* popped_lval = pop_lval(v, i);
	delete_lval(v);
	return popped_lval;
}

lval* join_lval(lval* x, lval* y) {
	for (int i = 0; i < y->count; i++) {
		append_lval(x, pop_lval(y, 0));
	}
	delete_lval(y);
	return x;
}

void delete_lval(lval* v) {
	if (v->type == LVAL_ERR) {
		free(v->err);
	}

	if (v->type == LVAL_SYM) {
		free(v->sym);
	}

	if (v->type == LVAL_SEXPR || v->type == LVAL_QEXPR) {
		for (int i = 0; i < v->count; i++) {
			delete_lval(v->cell[i]);
		}
		free(v->cell);
	}

	free(v);
}

void delete_lvals(int numArgs, ...) {
	va_list args;
	va_start(args, numArgs);

	for (int i = 0; i < numArgs; i++) {
		delete_lval(va_arg(args, lval*));
	}

	va_end(args);
}

void print_lval_expr(lval* v, char open, char close) {
	putchar(open);

	for (int i = 0; i < v->count; i++) {
		print_lval(v->cell[i]);
		if (i != v->count - 1) putchar(' ');
	}

	putchar(close);
}

void print_lval(lval* v) {
	if (v->type == LVAL_NUM) { printf("%li", v->num); }
	if (v->type == LVAL_ERR) { printf("%s", v->err); }
	if (v->type == LVAL_SYM) { printf("%s", v->sym); }
	if (v->type == LVAL_SEXPR) { print_lval_expr(v, '(', ')'); }
	if (v->type == LVAL_QEXPR) { print_lval_expr(v, '{', '}'); }
}

void println_lval(lval* v) {
	print_lval(v);
	putchar('\n');
}

