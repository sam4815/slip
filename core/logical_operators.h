#include "lval_definitions.h"

lval* true_lval(lenv* e, lval* v);
lval* false_lval(lenv* e, lval* v);

lval* and(lenv* e, lval* v);
lval* or(lenv* e, lval* v);
lval* not(lenv* e, lval* v);
