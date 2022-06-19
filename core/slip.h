#include "mpc/mpc.h"
#include "lval_definitions.h"

typedef char*(*pfunc)(lenv*, char*);
typedef void(*dfunc)(lenv*);

struct slip;
typedef struct slip slip;

struct slip {
	lenv* environment;
  mpc_parser_t* parser;
  pfunc evaluate_string;
  dfunc destroy;
};

slip* initialize_slip(void);
lval* evaluate_lval(lenv* env, lval* val);
char* evaluate_string(lenv* e, char* input);
char* slip_stringify(lval* result);

mpc_parser_t* get_parser(void);

static mpc_parser_t* Parser;
static mpc_parser_t* Expression;
static mpc_parser_t* Sexpression;
static mpc_parser_t* Qexpression;
static mpc_parser_t* Symbol;
static mpc_parser_t* String;
static mpc_parser_t* Comment;
static mpc_parser_t* Boolean;
static mpc_parser_t* Number;
