#include "lval_definitions.h"
#include "environment.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval* lval_num(long num)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = num;
    return v;
}

lval* lval_bool(int boole)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_BOOL;
    v->boole = boole;
    return v;
}

lval* lval_err(char* fmt, ...)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    va_list args;
    va_start(args, fmt);

    v->err = malloc(512);
    vsnprintf(v->err, 511, fmt, args);
    v->err = realloc(v->err, strlen(v->err) + 1);

    va_end(args);

    return v;
}

lval* lval_sym(char* sym)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(sym) + 1);
    strcpy(v->sym, sym);
    return v;
}

lval* lval_str(char* str)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_STR;
    v->str = malloc(strlen(str) + 1);
    strcpy(v->str, str);
    return v;
}

lval* lval_sexpr(void)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_qexpr(void)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_func(lfunc func)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUNC;
    v->func = func;
    return v;
}

lval* lval_lambda(lval* arguments, lval* body)
{
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUNC;
    v->func = NULL;

    v->env = initialize_env();

    v->arguments = arguments;
    v->body = body;

    return v;
}
