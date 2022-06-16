#include "lval_definitions.h"

lval* lambda(lenv* e, lval* v);
lval* call_lambda(lenv* env, lval* func, lval* arguments);
