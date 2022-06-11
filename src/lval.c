#include <stdio.h>
#include <stdarg.h>
#include "../mpc/mpc.h"
#include "lval.h"
#include "lenv.h"

lval* lval_num(long num) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num = num;
	return v;
}

lval* lval_err(char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
	v->type = LVAL_ERR;

  va_list args;
	va_start(args, fmt);

	v->err = malloc(512);
  vsnprintf(v->err, 511, fmt, args);
  v->err = realloc(v->err, strlen(v->err) + 1);

	va_end(args);

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

lval* lval_func(lfunc func) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_FUNC;
	v->func = func;
	return v;
}

lval* lval_lambda(lval* arguments, lval* body) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUNC;
  v->func = NULL;

  v->env = initialize_env();

  v->arguments = arguments;
  v->body = body;

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
  int count = y->count;
	for (int i = 0; i < count; i++) {
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

  // If user-defined lambda
  if (v->type == LVAL_FUNC && v->func == NULL) {
    delete_env(v->env);
    delete_lvals(2, v->arguments, v->body);
  }

	free(v);
}

lval* copy_lval(lval* v) {
	lval* x = malloc(sizeof(lval));
	x->type = v->type;

	if (v->type == LVAL_NUM) { x->num = v->num; }

	if (v->type == LVAL_SYM) {
		x->sym = malloc(strlen(v->sym) + 1);
		strcpy(x->sym, v->sym);
	}

	if (v->type == LVAL_ERR) {
		x->err = malloc(strlen(v->err) + 1);
		strcpy(x->err, v->err);
	}

	if (v->type == LVAL_FUNC && v->func != NULL) { x->func = v->func; }
  if (v->type == LVAL_FUNC && v->func == NULL) {
    x->func = NULL;
    x->env = copy_env(v->env);
    x->arguments = copy_lval(v->arguments);
    x->body = copy_lval(v->body);
  }

	if (v->type == LVAL_QEXPR || v->type == LVAL_SEXPR) {
		x->count = v->count;
		x->cell = malloc(sizeof(lval) * x->count);

		for (int i = 0; i < x->count; i++) {
			x->cell[i] = copy_lval(v->cell[i]);
		}
	}

	return x;
}

void delete_lvals(int numArgs, ...) {
	va_list args;
	va_start(args, numArgs);

	for (int i = 0; i < numArgs; i++) {
		delete_lval(va_arg(args, lval*));
	}

	va_end(args);
}

char* stringify_type(int t) {
  switch(t) {
    case LVAL_NUM: return "number";
    case LVAL_ERR: return "error";
    case LVAL_SEXPR: return "s-expression";
    case LVAL_QEXPR: return "q-expression";
    case LVAL_SYM: return "symbol";
    case LVAL_FUNC: return "function";
    default: return "unknown type";
  }
}

void print_lval_expr(lval* v, char open, char close) {
	putchar(open);

	for (int i = 0; i < v->count; i++) {
		print_lval(v->cell[i]);
		if (i != v->count - 1) putchar(' ');
	}

	putchar(close);
}

void print_lambda(lval* v) {
  printf("(\\ ");
  print_lval(v->arguments);
  putchar(' ');
  print_lval(v->body);
  putchar(')');
}

void print_lval(lval* v) {
	if (v->type == LVAL_NUM) { printf("%li", v->num); }
	if (v->type == LVAL_ERR) { printf("%s", v->err); }
	if (v->type == LVAL_SYM) { printf("%s", v->sym); }
	if (v->type == LVAL_SEXPR) { print_lval_expr(v, '(', ')'); }
	if (v->type == LVAL_QEXPR) { print_lval_expr(v, '{', '}'); }
	if (v->type == LVAL_FUNC && v->func != NULL) { printf("<built_in_function>"); }
	if (v->type == LVAL_FUNC && v->func == NULL) { print_lambda(v); }
}

void println_lval(lval* v) {
  printf("\033[33m");
	print_lval(v);
	printf("\033[0m\n");
}
