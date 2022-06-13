struct lval;
struct lenv;
typedef struct lenv lenv;
typedef struct lval lval;

typedef lval*(*lfunc)(lenv*, lval*);

enum { LVAL_NUM, LVAL_ERR, LVAL_SEXPR, LVAL_QEXPR, LVAL_SYM, LVAL_FUNC };

struct lval {
	int type;

  // Primitives
	long num;
	char* err;
	char* sym;

  // Built-in functions
	lfunc func;

  // User-defined functions
  lenv* env;
  lval* arguments;
  lval* body;

  // Expressions
	int count;
	struct lval** cell;
};

lval* lval_num(long num);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* sym);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_func(lfunc func);
lval* lval_lambda(lval* arguments, lval* body);
