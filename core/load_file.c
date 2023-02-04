#include "assert.h"
#include "lval_operations.h"
#include "lval_parser.h"
#include "slip.h"
#include <stdio.h>
#include <stdlib.h>

lval* load_file(lenv* e, lval* v)
{
    ASSERT_NUM_ARGS(v, 1, "load_file");
    ASSERT_CHILD_TYPE(v, LVAL_STR, 0, "load_file");

    FILE* slip_file = fopen(v->cell[0]->str, "rb");

    if (slip_file == NULL) {
        delete_lval(v);
        return lval_err("Could not open file.");
    }

    fseek(slip_file, 0, SEEK_END);
    long length = ftell(slip_file);
    fseek(slip_file, 0, SEEK_SET);

    char* input = calloc(length + 1, 1);
    fread(input, 1, length, slip_file);

    fclose(slip_file);

    lval* parsed_lval = read_string(input);
    free(input);

    lval* message = evaluate_lval(e, parsed_lval);

    return message;
};
