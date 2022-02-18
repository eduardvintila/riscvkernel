#ifndef __MM_H__
#define __MM_H__

#include <kernel/types.h>
#include <sbi/riscv_encoding.h>

#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(_AC(1, UL) << PAGE_SHIFT)


// A collection of bit-fields which describe a memory page.
struct page {
    // Whether this page is allocated or not.
    unsigned int free : 1;
    // Whether this is the last page in an allocation.
    // Useful for finding the length of a previously allocated sequence of pages when freeing.
    unsigned int last : 1;
};


// Used for traversing the list of free objects in the slab.
struct free_slab_obj {
    struct free_slab_obj *next;
};

// Slab structure which describes a region where objects can be allocated in fixed size chunks.
struct slab {
    // Size of each object in the slab.
    int size;

    // Head of the list of free objects in the slab.
    struct free_slab_obj *freelist_head;

    // Pointer to the next slab.
    struct slab *next_slab;

    // Starting address of the slab.
    physical_addr_t start;
};

void init_mm(void);
void *align(void*, unsigned int);
void *page_alloc(unsigned int);
void page_free(void*);
// void *alloc_slab_obj(struct slab *s);
// struct slab *alloc_slab(unsigned int size);
void *kalloc(unsigned int bytes);
void kfree(void *addr);

#endif
