struct lenv {
	int count;
	lval** vals;
	char** syms;

  lenv* parent;
};

lenv* initialize_env(void);
void delete_env(lenv* env);
lenv* copy_env(lenv* env);

lval* get_lval_from_env(lenv* env, lval* sym);
void set_lval_on_env(lenv* env, lval* sym, lval* val);
void set_global_lval(lenv* env, lval* sym, lval* val);

void set_func(lenv* env, char* name, lfunc func);
