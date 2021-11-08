/* Memory Management */

#include <kernel/mm.h>
#include <stdio.h>

extern unsigned int _heap_size;
extern unsigned int _heap_start;

static unsigned long long heap_size;
static void *heap_start;
static unsigned int heap_pages;

static void *alloc_start;
static unsigned int alloc_start_page_idx;
static unsigned int alloc_pages;

void *align(void *addr, unsigned int power_of_two)
{
    return (void *) (((unsigned long long)addr + power_of_two - 1) & ~(power_of_two - 1));
}

/* Initialize the memory structures. */
void init_mm(void)
{
    heap_size = (unsigned long long) &_heap_size;
    heap_start = &_heap_start;
    struct page *p = heap_start;

    heap_pages = heap_size / PAGE_SIZE;
    alloc_start = align(heap_start + heap_pages * sizeof(struct page), PAGE_SIZE);
    alloc_start_page_idx = ((unsigned long long)alloc_start - (unsigned long long)heap_start) / PAGE_SIZE;
    alloc_pages = heap_pages - alloc_start_page_idx;
    for (unsigned int i = 0; i < alloc_start_page_idx; i++) {
        p[i].free = 0;
        p[i].last = 0;
    }
    for (unsigned int i = alloc_start_page_idx; i < heap_pages; i++) {
        p[i].free = 1;
        p[i].last = 0;
    }
}

void *page_alloc(unsigned int pages_to_alloc)
{
    // TODO: Comments!
    // TODO: Refactor names!

    struct page *p = heap_start;
    void *addr;
    int start_page_idx = -1;
    unsigned int i, j;

    for (i = alloc_start_page_idx; i < heap_pages; i++) {
        if (p[i].free) {
            unsigned int nr = 1;
            for (j = i + 1; nr < pages_to_alloc && j < heap_pages; nr++, j++) {
                if (!p[j].free)
                    break;
            }
            if (nr < pages_to_alloc) {
                i = j;
            }
            else {
                addr = (char *) heap_start + i * PAGE_SIZE;
                start_page_idx = i;
                break;
            }
        }
    }

    if (start_page_idx == -1)
        return (void *)0;

    p = heap_start;
    for (i = start_page_idx; i < start_page_idx + pages_to_alloc; i++) {
        p[i].free = 0;
        if (i == start_page_idx + pages_to_alloc - 1)
            p[i].last = 1;
    }


    return addr;
}

void page_free(void *addr)
{
    unsigned int addr_page_idx = ((unsigned long long)addr - (unsigned long long)heap_start) / PAGE_SIZE;
    struct page *p = heap_start;
    unsigned int i = addr_page_idx;

    while (i < heap_pages && !p[i].last)
        p[i++].free = 1;

    if (i < heap_pages) {
        p[i].free = 1;
        p[i].last = 0;
    }
    //printf("Start page index: %u\n", start_page_idx);
}





