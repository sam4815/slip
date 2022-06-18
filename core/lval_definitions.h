#ifndef lval_definitions
#define lval_definitions

struct lval;
struct lenv;
typedef struct lenv lenv;
typedef struct lval lval;

typedef lval*(*lfunc)(lenv*, lval*);

enum {
  LVAL_NUM,
  LVAL_STR,
  LVAL_SYM,
  LVAL_ERR,
  LVAL_BOOL,
  LVAL_SEXPR,
  LVAL_QEXPR,
  LVAL_FUNC,
};

struct lval {
	int type;

  // Primitives
	long num;
  int boole;
	char* err;
	char* sym;
	char* str;

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
lval* lval_bool(int boole);
lval* lval_str(char* str);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* sym);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_func(lfunc func);
lval* lval_lambda(lval* arguments, lval* body);

#endif
