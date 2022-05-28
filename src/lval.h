typedef struct lval {
	int type;
	long num;

	char* err;
	char* sym;

	int count;
	struct lval** cell;
} lval;

enum { LVAL_NUM, LVAL_ERR, LVAL_SEXPR, LVAL_SYM };

lval* lval_num(long num);
lval* lval_err(char* err);
lval* lval_sym(char* sym);
lval* lval_sexpr(void);

lval* parse_lval(mpc_ast_t* tree);
lval* pop_lval(lval* v, int i);
lval* extract_lval(lval* v, int i);
void delete_lval(lval* v);
void delete_lvals(int numArgs, ...);

void print_lval_expr(lval* v, char open, char close);
void print_lval(lval* v);
void println_lval(lval* v);

