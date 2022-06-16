#include <stdio.h>
#include <stdlib.h>
#include "../core/slip.h"
#include "testcases.h"

#define GREEN "\033[1;32m"
#define RED "\033[31;1m"
#define RESET "\033[0m"

int main(int argc, char** argv) {
	slip* slippy = initialize_slip();

  testcases* tests = get_testcases();

  for (int i = 0; i < tests->count; i++) {
    char* expected = tests->tests[i]->expected;
    char* message;
    
    for (int j = 0; j < tests->tests[i]->input_count; j++) {
      char* line = tests->tests[i]->input[j];
      message = slippy->evaluate_string(slippy->parser, slippy->environment, line);
    }

    if (strcmp(message, expected) != 0) {
      printf(RED "Test failure in `%s`.\nExpecting `%s`, received `%s`.\n" RESET,
        tests->tests[i]->name, expected, message);
      return -1;
    }
  }
  
  slippy->destroy(slippy->parser, slippy->environment);

  printf(GREEN "✨ All test cases passed.\n" RESET);
	return 0;
}
