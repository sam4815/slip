#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc/mpc.h"
#include "lval_stringify.h"

char* stringify_type(int t) {
  switch(t) {
    case LVAL_NUM: return "number";
    case LVAL_ERR: return "error";
    case LVAL_STR: return "string";
    case LVAL_SEXPR: return "s-expression";
    case LVAL_QEXPR: return "q-expression";
    case LVAL_SYM: return "symbol";
    case LVAL_FUNC: return "function";
    case LVAL_BOOL: return "boolean";
    default: return "unknown type";
  }
}

char* stringify_lambda(lval* v) {
  char* arguments_str = stringify_lval(v->arguments);
  char* body_str = stringify_lval(v->body);

  char* result = malloc(strlen(arguments_str) + strlen(body_str) + 4);
  strcpy(result, arguments_str);
  strcat(result, " (");
  strcat(result, body_str);
  strcat(result, ")");

  return result;
}

char* stringify_lval_expression(lval* v, char* open, char* close) {
  if (v->count == 0) { return ""; }

  char* result = malloc(strlen(open) + 1);
  strcpy(result, open);

	for (int i = 0; i < v->count; i++) {
    char* child_string = stringify_lval(v->cell[i]);
    result = realloc(result, strlen(result) + strlen(child_string) + 1);
    strcat(result, child_string);

		if (i != v->count - 1) {
      result = realloc(result, strlen(result) + 2);
      strcat(result, " ");
    }
	}

	result = realloc(result, strlen(result) + strlen(close) + 1);
  strcat(result, close);

  return result;
}

char* stringify_lval(lval* v) {
	if (v->type == LVAL_NUM) {
    char* c = malloc(32);
    sprintf(c, "%li", v->num);
    return c;
  }

  if (v->type == LVAL_BOOL) {
    char* c = malloc(6);
    sprintf(c, v->boole ? "true" : "false");
    return c;
  }

  if (v->type == LVAL_ERR) {
    char* c = malloc(strlen(v->err) + 1);
    sprintf(c, "%s", v->err);
    return c;
  }

  if (v->type == LVAL_SYM) {
    char* c = malloc(strlen(v->sym) + 1);
    sprintf(c, "%s", v->sym);
    return c;
  }

  if (v->type == LVAL_STR) {
    char* c = malloc(strlen(v->str) + 1);
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);
    escaped = mpcf_escape(escaped);
    sprintf(c, "\"%s\"", escaped);
    return c;
  }

  if (v->type == LVAL_FUNC && v->func != NULL) {
    char* c = malloc(20);
    sprintf(c, "<built_in_function>");
    return c;
  }

	if (v->type == LVAL_SEXPR) { return stringify_lval_expression(v, "(", ")"); }
	if (v->type == LVAL_QEXPR) { return stringify_lval_expression(v, "{", "}"); }	
	if (v->type == LVAL_FUNC && v->func == NULL) { return stringify_lambda(v); }

  char* c = malloc(21);
  sprintf(c, "Unable to stringify.");
  return c;
}
