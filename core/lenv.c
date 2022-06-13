#include <stdlib.h>
#include <stdio.h>
#include "mpc/mpc.h"
#include "lval.h"
#include "lenv.h"

lenv* initialize_env(void) {
	lenv* e = malloc(sizeof(lenv));
	e->count = 0;
	e->syms = NULL;
	e->vals = NULL;
  e->parent = NULL;

	return e;
}

void delete_env(lenv* e) {
	for (int i = 0; i < e->count; i++) {
		free(e->syms[i]);
		delete_lval(e->vals[i]);
	}

	free(e->syms);
	free(e->vals);
	free(e);
}

lenv* copy_env(lenv* e) {
  lenv* x = malloc(sizeof(lenv));
  x->count = e->count;
  x->parent = e->parent;
  x->syms = malloc(sizeof(char*) * e->count);
  x->vals = malloc(sizeof(lval*) * e->count);
  
  for (int i = 0; i < e->count; i++) {
    x->vals[i] = copy_lval(e->vals[i]);
    x->syms[i] = malloc(strlen(e->syms[i]) + 1);
	  strcpy(x->syms[i], e->syms[i]);
  }

  return x;
}

lval* get_lval(lenv* e, lval* sym) {
	for (int i = 0; i < e->count; i++) {
		if (strcmp(e->syms[i], sym->sym) == 0) {
			return copy_lval(e->vals[i]);
		}
	}

  if (e->parent != NULL) {
    return get_lval(e->parent, sym);
  }

	return lval_err("Symbol '%s' not found", sym->sym);
}

void set_lval(lenv* e, lval* sym, lval* val) {
	for (int i = 0; i < e->count; i++) {
		if (strcmp(e->syms[i], sym->sym) == 0) {
			delete_lval(e->vals[i]);
			e->vals[i] = copy_lval(val);
			return;
		}
	}
	
	e->count = e->count + 1;
	e->syms = realloc(e->syms, sizeof(lval*) * e->count);
	e->vals = realloc(e->vals, sizeof(char*) * e->count);

	e->vals[e->count - 1] = copy_lval(val);
	e->syms[e->count - 1] = malloc(strlen(sym->sym) + 1);
	strcpy(e->syms[e->count - 1], sym->sym);
}

void set_global_lval(lenv* e, lval* sym, lval* val) {
  while (e->parent) { e = e->parent; }

  set_lval(e, sym, val);
}

void set_func(lenv* e, char* name, lfunc func) {
	lval* sym = lval_sym(name);
	lval* val = lval_func(func);
	set_lval(e, sym, val);
	delete_lval(sym);
  delete_lval(val);
}
