#define ASSERT(val, cond, fmt, ...) \
	if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); delete_lval(val); return err; }

#define ASSERT_NUM_ARGS(val, num, name) \
  ASSERT(val, val->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, expected %i.", name, val->count, num);

#define ASSERT_TYPE(val, target, name) \
  ASSERT(val, val->type == target, \
    "Function '%s' passed incorrect type. Got %s, expected %s.", name, stringify_type(val->type), stringify_type(target));

#define ASSERT_CHILD_NOT_EMPTY(val, name) \
  ASSERT(val, val->cell[0]->count != 0, "Function '%s' passed empty list.", name);

#define ASSERT_CHILD_TYPE(val, target, index, name) \
  ASSERT(val, val->cell[index]->type == target, \
    "Function '%s' passed incorrect type. Got %s, expected %s.", name, stringify_type(val->cell[index]->type), stringify_type(target));

#define ASSERT_VALID_QEXPR_ARG(name, val) \
  ASSERT_NUM_ARGS(val, 1, name); \
  ASSERT_CHILD_TYPE(val, LVAL_QEXPR, 0, name); \
  ASSERT_CHILD_NOT_EMPTY(val, name);

#define ASSERT_VALID_DEF(name, val) \
  ASSERT_CHILD_TYPE(v, LVAL_QEXPR, 0, "def"); \
  ASSERT_CHILD_NOT_EMPTY(v, "def"); \
  ASSERT_NUM_ARGS(v, v->cell[0]->count + 1, "def");

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
lval* lval_func(lfunc func);
lval* lval_lambda(lval* arguments, lval* body);

lval* parse_lval(mpc_ast_t* tree);
lval* pop_lval(lval* v, int i);
lval* join_lval(lval* x, lval* y);
lval* append_lval(lval* parent, lval* child);
lval* extract_lval(lval* v, int i);
lval* copy_lval(lval* v);
void delete_lval(lval* v);
void delete_lvals(int numArgs, ...);

char* stringify_type(int type);

void print_lval_expr(lval* v, char open, char close);
void print_lval(lval* v);
void println_lval(lval* v);
