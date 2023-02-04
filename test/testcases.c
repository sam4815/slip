#include "testcases.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

testcases* get_testcases(void)
{
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

    add_testcase(tests, "Conditional statements", "3",
        "if (== 1 1) {+ 1 2} {+ 1 3}",
        0);

    add_testcase(tests, "Conditional statements", "4",
        "if (== 1 2) {+ 1 2} {+ 1 3}",
        0);

    add_testcase(tests, "Logical operators: and", "true",
        "and (== 10 10) (== 10 10)",
        0);

    add_testcase(tests, "Logical operators: and", "true",
        "and true true",
        0);

    add_testcase(tests, "Logical operators: and", "false",
        "and (== 10 10) (== 10 12)",
        0);

    add_testcase(tests, "Logical operators: or", "true",
        "or (== 10 10) (== 10 12)",
        0);

    add_testcase(tests, "Logical operators: or", "false",
        "or (== 10 11) (== 10 12)",
        0);

    add_testcase(tests, "Logical operators: or", "false",
        "or false false",
        0);

    add_testcase(tests, "Logical operators: not", "false",
        "not (== 10 10)",
        0);

    add_testcase(tests, "Logical operators: not", "false",
        "not true",
        0);

    add_testcase(tests, "Strings", "\"hello\"",
        "eval (head {\"hello\" \"world\"})",
        0);

    add_testcase(tests, "Library functions loaded", "30",
        "fun {add-ten a} {+ a 10}",
        "add-ten 20",
        0);

    add_testcase(tests, "Returns an error when row isn't in database", "Row not found.",
        "select 3",
        0);

    add_testcase(tests, "Insert and select database row", "\"34 Full Name email@email.com\"",
        "insert 34 \"Full Name\" \"email@email.com\"",
        "select 34",
        0);

    char long_name[] = "Max name length (32), max name l";
    char long_name_insert_statement[100];
    char long_name_select_result[100];
    sprintf(long_name_insert_statement, "(insert 2 \"%s\" \"test@testing.com\")", long_name);
    sprintf(long_name_select_result, "\"2 %s test@testing.com\"", long_name);
    add_testcase(tests, "Allows inserting strings that are the maximum length", long_name_select_result,
        long_name_insert_statement,
        "select 2",
        0);

    char too_long_name[] = "Max name length (33), max name le";
    char too_long_name_insert_statement[100];
    sprintf(too_long_name_insert_statement, "(insert 53 \"%s\" \"test@testing.com\")", too_long_name);
    add_testcase(tests, "Allows inserting strings that are the maximum length", "Username too long.",
        too_long_name_insert_statement,
        0);

    char insert_statements[50000];
    for (int i = 1; i < 1001; i++) {
        char* insert_statement = malloc(50);
        sprintf(insert_statement, "(insert %i \"%i\" \"%i@%i.com\")", i, i, i, i);
        strcat(insert_statements, insert_statement);
    }

    add_testcase(tests, "Accepts hundreds of rows", "\"998 998 998@998.com\"",
        insert_statements,
        "select 998",
        0);

    char full_statement[70000];
    for (int i = 1; i < 1400; i++) {
        char* insert_statement = malloc(50);
        sprintf(insert_statement, "(insert %i \"test\" \"test@testing.com\")", i);
        strcat(full_statement, insert_statement);
    }

    add_testcase(tests, "Returns an error if the database is full", "Table is full.",
        full_statement,
        0);

    return tests;
}
