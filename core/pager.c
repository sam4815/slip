#include "pager.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void pager_flush(Pager* pager, uint32_t page_num, uint32_t page_size)
{
    lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    write(pager->file_descriptor, pager->pages[page_num], page_size);
}
