#include "database.h"
#include "assert.h"
#include "lval_operations.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

Table* TABLE = NULL;

void serialize_row(Row* source, void* destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

Pager* pager_open()
{
    int file_descriptor = open("build/.db",
        O_RDWR | O_CREAT,
        S_IWUSR | S_IRUSR);

    if (file_descriptor == -1) {
        printf("Unable to open database file.");
        exit(1);
    }

    off_t file_length = lseek(file_descriptor, 0, SEEK_END);

    Pager* pager = malloc(sizeof(Pager));
    pager->file_descriptor = file_descriptor;
    pager->file_length = file_length;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }

    return pager;
}

void* get_page(Pager* pager, uint32_t page_num)
{
    // If it's not in the cache, load from file
    if (pager->pages[page_num] == NULL) {
        void* page = malloc(PAGE_SIZE);

        lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
        read(pager->file_descriptor, page, PAGE_SIZE);
        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void* row_memory_location(Table* t, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;

    void* page = get_page(t->pager, page_num);

    return page + byte_offset;
}

void pager_flush(Pager* pager, uint32_t page_num, uint32_t page_size)
{
    lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    write(pager->file_descriptor, pager->pages[page_num], page_size);
}

void db_close()
{
    if (TABLE == NULL) {
        return;
    }

    Pager* pager = TABLE->pager;
    uint32_t num_full_pages = TABLE->num_rows / ROWS_PER_PAGE + 1;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }

        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    close(pager->file_descriptor);
    free(pager);
    free(TABLE);
}

Table* create_table()
{
    Pager* pager = pager_open();
    uint32_t num_rows = pager->file_length / ROW_SIZE;

    Table* t = malloc(sizeof(Table));
    t->pager = pager;
    t->num_rows = num_rows;

    return t;
}

Row* create_row(uint32_t id, char* username, char* email)
{
    Row* r = malloc(sizeof(Row));
    r->id = id;
    strcpy(r->username, username);
    strcpy(r->email, email);
    return r;
}

lval* select_record(lenv* e, lval* v)
{
    ASSERT_NUM_ARGS(v, 1, "select");
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, "select");

    if (TABLE == NULL) {
        TABLE = create_table();
    }

    lval* row_id = pop_lval(v, 0);
    Row* curr_row = create_row(0, "", "");

    for (int i = 0; i < TABLE->num_rows; i++) {
        deserialize_row(row_memory_location(TABLE, i), curr_row);

        if (curr_row->id == row_id->num) {
            char* selected_row = malloc(sizeof(Row));
            sprintf(selected_row, "%d %s %s", curr_row->id, curr_row->username, curr_row->email);
            return lval_str(selected_row);
        }
    }

    return lval_err("Row not found.");
}

lval* insert_record(lenv* e, lval* v)
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

    Row* row_to_insert = create_row(id->num, username->str, email->str);

    void* row_location = row_memory_location(TABLE, TABLE->num_rows);

    serialize_row(row_to_insert, row_location);
    TABLE->num_rows += 1;

    return id;
}

lval* delete_record(lenv* e, lval* v)
{
    ASSERT_CHILD_TYPE(v, LVAL_NUM, 0, "delete");
    return lval_err("Not implemented.");
}
