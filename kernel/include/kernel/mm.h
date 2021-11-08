#ifndef __MM_H__
#define __MM_H__

#define PAGE_SIZE 4096

// A collection of bit-fields which describe a memory page.
struct page {
    // Whether this page is allocated or not.
    unsigned int free : 1;
    // Whether this is the last page in an allocation.
    // Useful for finding the length of a previously allocated sequence of pages when freeing.
    unsigned int last : 1;
};

void init_mm(void);
void *align(void*, unsigned int);
void *page_alloc(unsigned int);
void page_free(void*);

#endif
