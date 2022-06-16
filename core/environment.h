#include "lval_definitions.h"

struct lenv {
	int count;
	lval** vals;
	char** syms;

  lenv* parent;
};

lenv* initialize_env(void);
lenv* copy_env(lenv* env);
void delete_env(lenv* env);

lval* get_lval_from_env(lenv* env, lval* sym);
void set_lval_on_env(lenv* env, lval* sym, lval* val);
void set_func_on_env(lenv* env, char* name, lfunc func);
void set_global_lval(lenv* env, lval* sym, lval* val);
