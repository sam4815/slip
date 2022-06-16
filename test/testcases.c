#include "testcases.h"
#include "helpers.h"

testcases* get_testcases(void) {
  testcases* tests = initialize_testcases();

  add_testcase(tests, "Addition", "20",
    "+ 10 10",
  0);

  add_testcase(tests, "Combining s-expressions", "54",
    "- (+ 5 (* 6 9)) (/ 10 (+ 1 1))",
  0);

  add_testcase(tests, "Q-expression manipulation: tail", "{2 3 4}",
    "tail {1 2 3 4}",
  0);

  add_testcase(tests, "Q-expression manipulation: eval", "7",
    "eval {+ 3 4}",
  0);

  add_testcase(tests, "Q-expression manipulation: join", "{5 66 3 22 21}",
    "join {5} {66 3} {22 21}",
  0);

  add_testcase(tests, "Q-expression manipulation: len", "5",
    "len (join {5} {66 3} {22 21})",
  0);

  add_testcase(tests, "Variable definition", "10",
    "def {x} 10",
    "x",
  0);

  add_testcase(tests, "Lambda definition", "310",
    "def {add-together} (\\ {x y} {+ x y})",
    "add-together 300 10",
  0);

  add_testcase(tests, "Partial evaluation", "510",
    "def {add-mul} (\\ {x y} {+ x (* x y)})",
    "def {add-mul-ten} (add-mul 10)",
    "add-mul-ten 50",
  0);

  add_testcase(tests, "Currying", "18",
    "fun {curry f xs} {eval (join (list f) xs)}",
    "curry + {5 6 7}",
  0);

  add_testcase(tests, "Uncurrying", "{5}",
    "fun {uncurry f & xs} {f xs}",
    "uncurry head 5 6 7",
  0);

  add_testcase(tests, "Comparing numbers", "true",
    "== 516 516",
  0);

  add_testcase(tests, "Comparing numbers", "true",
    "!= 515 516",
  0);

  add_testcase(tests, "Comparing numbers", "true",
    "> 516 216",
  0);

  add_testcase(tests, "Comparing numbers", "false",
    "< 516 216",
  0);

  add_testcase(tests, "Comparing q-expressions", "true",
    "== {1 2 5} {1 2 5}",
  0);

  add_testcase(tests, "Comparing s-expressions", "true",
    "== (+ 2 6) (+ 4 4)",
  0);

  add_testcase(tests, "Comparing functions", "true",
    "fun {add_1 a b} {+ a b}",
    "fun {add_2 a b} {+ a b}",
    "== add_1 add_2",
  0);

  add_testcase(tests, "Comparing functions", "false",
    "fun {add a b} {+ a b}",
    "fun {subtract a b} {- a b}",
    "== add subtract",
  0);

  add_testcase(tests, "Comparing functions", "true",
    "== head head",
  0);

  add_testcase(tests, "Condition statements", "3",
    "if (== 1 1) {+ 1 2} {+ 1 3}",
  0);

  add_testcase(tests, "Condition statements", "4",
    "if (== 1 2) {+ 1 2} {+ 1 3}",
  0);

  return tests;
}
