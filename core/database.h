#include "lval_definitions.h"
#include <stdint.h>

#define size_of_attribute(struct, attribute) sizeof(((struct*)0)->attribute)
#define TABLE_MAX_PAGES 100

lval* insert(lenv* e, lval* v);
lval* select(lenv* e, lval* v);
lval* delete(lenv* e, lval* v);

typedef struct {
    uint32_t id;
    char username[32];
    char email[255];
} row;

typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} table;
