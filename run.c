#include <stdio.h>
#include <emscripten/emscripten.h>
#include "../core/mpc/mpc.h"
#include "../core/lval_definitions.h"
#include "../core/environment.h"
#include "../core/slip.h"

slip* slippy;
extern slip* slippy;

int main() {
  slippy = initialize_slip();
}

EMSCRIPTEN_KEEPALIVE
char* evaluate(char* input) {
  return slippy->evaluate_string(slippy->parser, slippy->environment, input);
}
