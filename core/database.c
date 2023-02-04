#include "database.h"
#include "assert.h"
#include "lval_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint32_t ID_SIZE = size_of_attribute(row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(row, email);
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

void serialize_row(row* source, void* destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, row* destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void* row_memory_location(table* t, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;

    if (t->pages[page_num] == NULL) {
        t->pages[page_num] = malloc(PAGE_SIZE);
    }

    return t->pages[page_num] + byte_offset;
}

table* create_table()
{
    table* t = malloc(sizeof(table));

    t->num_rows = 0;
    for (int i = 0; i < TABLE_MAX_PAGES; i++) {
        t->pages[i] = NULL;
    }

    return t;
}

void free_table(table* t)
{
    for (int i = 0; i < TABLE_MAX_PAGES; i++) {
        free(t->pages[i]);
    }
    free(t);
}

row* create_row(uint32_t id, char* username, char* email)
{
    row* r = malloc(sizeof(row));
    r->id = id;
    strcpy(r->username, username);
    strcpy(r->email, email);
    return r;
}

table* TABLE = NULL;

lval* select(lenv* e, lval* v)
{
    ASSERT_NUM_ARGS(v, 1, "select");
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, "select");

    if (TABLE == NULL) {
        TABLE = create_table();
    }

    lval* row_id = pop_lval(v, 0);
    row* curr_row = create_row(0, "", "");

    for (int i = 0; i < TABLE->num_rows; i++) {
        deserialize_row(row_memory_location(TABLE, i), curr_row);

        if (curr_row->id == row_id->num) {
            char* selected_row = malloc(sizeof(row));
            sprintf(selected_row, "%d %s %s", curr_row->id, curr_row->username, curr_row->email);
            return lval_str(selected_row);
        }
    }

    return lval_err("Row not found.");
}

lval* insert(lenv* e, lval* v)
{
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, "insert");
    ASSERT_CHILD_TYPE(v, LVAL_STR, 1, "insert");
    ASSERT_CHILD_TYPE(v, LVAL_STR, 2, "insert");
    ASSERT_MAX_NUM_ARGS(v, 3, "insert");

    if (TABLE == NULL) {
        TABLE = create_table();
    }

    if (TABLE->num_rows >= TABLE_MAX_ROWS) {
        return lval_err("Table is full.");
    }

    lval* id = pop_lval(v, 0);
    lval* username = pop_lval(v, 0);
    lval* email = pop_lval(v, 0);

    ASSERT_POSITIVE(id, "ID");
    ASSERT_LENGTH_LTE(username, USERNAME_MAX_LENGTH, "Username");
    ASSERT_LENGTH_LTE(email, EMAIL_MAX_LENGTH, "Email");

    row* row_to_insert = create_row(id->num, username->str, email->str);

    void* row_location = row_memory_location(TABLE, TABLE->num_rows);

    serialize_row(row_to_insert, row_location);
    TABLE->num_rows += 1;

    return id;
}

lval* delete(lenv* e, lval* v)
{
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, "delete");
    return lval_err("Not implemented.");
}
