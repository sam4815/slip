#include "mpc/mpc.h"
#include "lval_definitions.h"

typedef char*(*pfunc)(mpc_parser_t*, lenv*, char*);
typedef void(*dfunc)(mpc_parser_t*, lenv*);

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
char* evaluate_string(mpc_parser_t* Parser, lenv* e, char* input);
char* slip_stringify(lval* result);

mpc_parser_t* Parser;
mpc_parser_t* Expression;
mpc_parser_t* Sexpression;
mpc_parser_t* Qexpression;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Boolean;
mpc_parser_t* Number;
