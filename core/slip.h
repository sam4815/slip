#include "lval_definitions.h"

typedef char*(*pfunc)(lenv*, char*);
typedef void(*dfunc)(lenv*);

struct slip;
typedef struct slip slip;

struct slip {
	lenv* environment;
  pfunc evaluate_string;
  dfunc destroy;
};

slip* initialize_slip(void);
lval* evaluate_lval(lenv* env, lval* val);
char* evaluate_string(lenv* e, char* input);
char* slip_stringify(lval* result);
