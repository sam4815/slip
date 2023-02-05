#include "slip.h"
#include "database.h"
#include "environment.h"
#include "lambdas.h"
#include "library.h"
#include "lval_operations.h"
#include "lval_parser.h"
#include "lval_stringify.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

lval* call_lval(lenv* env, lval* func, lval* arguments)
{
    if (func->func) {
        return func->func(env, arguments);
    }

    return call_lambda(env, func, arguments);
}

lval* evaluate_lval(lenv* env, lval* val)
{
    // Return corresponding variable
    if (val->type == LVAL_SYM) {
        lval* env_val = get_lval_from_env(env, val);
        delete_lval(val);
        return env_val;
    }

    if (val->type != LVAL_SEXPR || val->count == 0) {
        return val;
    }

    // Evaluate all children
    for (int i = 0; i < val->count; i++) {
        val->cell[i] = evaluate_lval(env, val->cell[i]);
        if (val->cell[i]->type == LVAL_ERR) {
            return extract_lval(val, i);
        }
    }

    // If the first child isn't a function, just extract and return the final evaluated child
    if (val->cell[0]->type != LVAL_FUNC) {
        return extract_lval(val, val->count - 1);
    }

    // Assume val is an s-expression and perform the first child (a function) on the remaining children
    lval* func = pop_lval(val, 0);
    lval* result = call_lval(env, func, val);

    delete_lval(func);
    return result;
};

void destroy_slip(lenv* e)
{
    db_close();
    delete_env(e);
}

char* evaluate_string(lenv* e, char* input)
{
    lval* evaluation = evaluate_lval(e, read_string(input));
    char* evaluation_str = stringify_lval(evaluation);

    delete_lval(evaluation);

    return evaluation_str;
}

slip* initialize_slip(void)
{
    slip* slip_ptr = malloc(sizeof(slip));

    lenv* environment = initialize_env();
    build_library(environment);

    slip_ptr->environment = environment;
    slip_ptr->evaluate_string = evaluate_string;
    slip_ptr->destroy = destroy_slip;

    return slip_ptr;
}
