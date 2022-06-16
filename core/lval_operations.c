#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lval_operations.h"
#include "environment.h"

lval* append_lval(lval* parent, lval* child) {
	parent->count = parent->count + 1;
	parent->cell = realloc(parent->cell, sizeof(lval*) * parent->count);
	parent->cell[parent->count - 1] = child;
	return parent;
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
	if (v->type == LVAL_BOOL) { x->boole = v->boole; }

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
