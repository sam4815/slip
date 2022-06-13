lval* append_lval(lval* parent, lval* child);
lval* pop_lval(lval* v, int i);
lval* extract_lval(lval* v, int i);
lval* join_lval(lval* x, lval* y);
lval* copy_lval(lval* v);
void delete_lval(lval* v);
void delete_lvals(int numArgs, ...);
