#include "lval_definitions.h"
#include "pager.h"
#include <stdint.h>

#define size_of_attribute(struct, attribute) sizeof(((struct*)0)->attribute)
#define USERNAME_MAX_LENGTH 32
#define EMAIL_MAX_LENGTH 255

lval* insert_record(lenv* e, lval* v);
lval* select_record(lenv* e, lval* v);
lval* delete_record(lenv* e, lval* v);

void db_close();

typedef struct {
    uint32_t id;
    char username[USERNAME_MAX_LENGTH + 1];
    char email[EMAIL_MAX_LENGTH + 1];
} Row;

typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;
