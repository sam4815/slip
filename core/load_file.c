#include "mpc/mpc.h"
#include "slip.h"
#include "assert.h"
#include "lval_parser.h"

lval* load_file(lenv* e, lval* v) {
  ASSERT_NUM_ARGS(v, 1, "load_file");
  ASSERT_CHILD_TYPE(v, LVAL_STR, 0, "load_file");

  mpc_result_t result;
  int parse_success = mpc_parse_contents(v->cell[0]->str, Parser, &result);

  if (!parse_success) {
    return lval_err(mpc_err_string(result.error));
  }

  lval* message = evaluate_lval(e, parse_lval(result.output));

  return message;
};
