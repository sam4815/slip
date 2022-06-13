#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "testcases.h"
#include "helpers.h"

testcases* add_testcase(testcases* tests, char* name, char* expected, ...) {
  testcase* test = malloc(sizeof(testcase));

  test->input = NULL;
  test->input_count = 0;

  test->name = malloc(strlen(name) + 1);
  strcpy(test->name, name);

  test->expected = malloc(strlen(expected) + 1);
  strcpy(test->expected, expected);

  va_list args;
	va_start(args, expected);

	while (1) {
    char* line = va_arg(args, char*);
    if (!line) { break; }

    test->input_count = test->input_count + 1;
    test->input = realloc(test->input, sizeof(char*) * test->input_count);

    test->input[test->input_count - 1] = malloc(strlen(line) + 1);
    strcpy(test->input[test->input_count - 1], line);
	}
  
	va_end(args);

  tests->count = tests->count + 1;
  tests->tests = realloc(tests->tests, sizeof(testcase*) * tests->count);
  tests->tests[tests->count - 1] = malloc(sizeof(testcase*));
  tests->tests[tests->count - 1] = test;

  return tests;
}

testcases* initialize_testcases(void) {
  testcases* tests = malloc(sizeof(testcases));
  tests->tests = NULL;
  tests->count = 0;
  return tests;
}
