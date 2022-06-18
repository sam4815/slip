#include "lval_operations.h"
#include "lval_parser.h"

lval* parse_lval_number(mpc_ast_t* tree) {
	errno = 0;
	long number = strtol(tree->contents, NULL, 10);
	return errno == ERANGE ? lval_err("Invalid number") : lval_num(number);
}

lval* parse_lval_bool(mpc_ast_t* tree) {
  if (strstr(tree->contents, "true")) return lval_bool(1);
  if (strstr(tree->contents, "false")) return lval_bool(0);
  return lval_err("Invalid boolean");
}

lval* parse_lval_string(mpc_ast_t* tree) {
  // Ignore first and last characters, which are quotation marks
  tree->contents[strlen(tree->contents) - 1] = '\0';
  char* unescaped_string = malloc(strlen(tree->contents));
  strcpy(unescaped_string, tree->contents + 1);

  unescaped_string = mpcf_unescape(unescaped_string);

  lval* result = lval_str(unescaped_string);
  free(unescaped_string);

  return result;
}

lval* parse_lval(mpc_ast_t* tree) {
	lval* parsed_lval = lval_sexpr();

	if (strstr(tree->tag, "number")) { parsed_lval = parse_lval_number(tree); }
	if (strstr(tree->tag, "boolean")) { parsed_lval = parse_lval_bool(tree); }
	if (strstr(tree->tag, "string")) { parsed_lval = parse_lval_string(tree); }
	if (strstr(tree->tag, "symbol")) { parsed_lval = lval_sym(tree->contents); }
	if (strstr(tree->tag, "qexpr")) { parsed_lval = lval_qexpr(); }
	if (strstr(tree->tag, "sexpr") || strcmp(tree->tag, ">") == 0) {
		parsed_lval = lval_sexpr();
	}

	for (int i = 0; i < tree->children_num; i++) {
		if (strcmp(tree->children[i]->contents, "(") == 0) { continue; }
		if (strcmp(tree->children[i]->contents, ")") == 0) { continue; }
		if (strcmp(tree->children[i]->contents, "}") == 0) { continue; }
		if (strcmp(tree->children[i]->contents, "{") == 0) { continue; }

		if (strcmp(tree->children[i]->tag, "regex") == 0) { continue; }

		parsed_lval = append_lval(parsed_lval, parse_lval(tree->children[i]));
	}

	return parsed_lval;
}
