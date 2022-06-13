struct testcase;
struct testcases;
typedef struct testcase testcase;
typedef struct testcases testcases;

struct testcases {
  int count;
  struct testcase** tests;
};

struct testcase {
  char* name;

  char* expected;

  int input_count;
  char** input;
};

testcases* get_testcases(void);
