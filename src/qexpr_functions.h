lval* head(lenv* e, lval* v);
lval* tail(lenv* e, lval* v);
lval* list(lenv* e, lval* v);
lval* eval(lenv* e, lval* v);
lval* join(lenv* e, lval* v);
lval* cons(lenv* e, lval* v);
lval* init(lenv* e, lval* v);
lval* len(lenv* e, lval* v);

lval* global_def(lenv* e, lval* v);
lval* local_def(lenv* e, lval* v);
lval* lambda(lenv* e, lval* v);
