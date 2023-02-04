#include "../core/slip.h"
#include <emscripten/emscripten.h>
#include <stdio.h>

slip* slippy;
extern slip* slippy;

int main()
{
    slippy = initialize_slip();
}

EMSCRIPTEN_KEEPALIVE
char* evaluate(char* input)
{
    return slippy->evaluate_string(slippy->environment, input);
}
