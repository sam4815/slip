struct lenv {
	int count;
	lval** vals;
	char** syms;
};

lenv* initialize_env(void);
void delete_env(lenv*);

lval* get_lval(lenv* e, lval* sym);
void set_lval(lenv* e, lval* sym, lval* val);

void set_func(lenv* e, char* name, lfunc func);

lval* print_env(lenv* e, lval* v);
