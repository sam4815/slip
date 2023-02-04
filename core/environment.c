#include "environment.h"
#include "lval_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lenv* initialize_env(void)
{
    lenv* env = malloc(sizeof(lenv));
    env->count = 0;
    env->syms = NULL;
    env->vals = NULL;
    env->parent = NULL;

    return env;
}

void delete_env(lenv* env)
{
    for (int i = 0; i < env->count; i++) {
        free(env->syms[i]);
        delete_lval(env->vals[i]);
    }

    free(env->syms);
    free(env->vals);
    free(env);
}

lenv* copy_env(lenv* env)
{
    lenv* x = malloc(sizeof(lenv));
    x->count = env->count;
    x->parent = env->parent;
    x->syms = malloc(sizeof(char*) * env->count);
    x->vals = malloc(sizeof(lval*) * env->count);

    for (int i = 0; i < env->count; i++) {
        x->vals[i] = copy_lval(env->vals[i]);
        x->syms[i] = malloc(strlen(env->syms[i]) + 1);
        strcpy(x->syms[i], env->syms[i]);
    }

    return x;
}

lval* get_lval_from_env(lenv* env, lval* sym)
{
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->syms[i], sym->sym) == 0) {
            return copy_lval(env->vals[i]);
        }
    }

    if (env->parent != NULL) {
        return get_lval_from_env(env->parent, sym);
    }

    return lval_err("Symbol '%s' not found", sym->sym);
}

void set_lval_on_env(lenv* env, lval* sym, lval* val)
{
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->syms[i], sym->sym) == 0) {
            delete_lval(env->vals[i]);
            env->vals[i] = copy_lval(val);
            return;
        }
    }

    env->count = env->count + 1;
    env->syms = realloc(env->syms, sizeof(lval*) * env->count);
    env->vals = realloc(env->vals, sizeof(char*) * env->count);

    env->vals[env->count - 1] = copy_lval(val);
    env->syms[env->count - 1] = malloc(strlen(sym->sym) + 1);
    strcpy(env->syms[env->count - 1], sym->sym);
}

void set_global_lval(lenv* env, lval* sym, lval* val)
{
    while (env->parent) {
        env = env->parent;
    }

    set_lval_on_env(env, sym, val);
}

void set_func_on_env(lenv* env, char* name, lfunc func)
{
    lval* sym = lval_sym(name);
    lval* val = lval_func(func);
    set_lval_on_env(env, sym, val);
    delete_lval(sym);
    delete_lval(val);
}
