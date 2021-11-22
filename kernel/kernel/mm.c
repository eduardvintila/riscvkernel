/* Memory Management */

#include <kernel/mm.h>
#include <stdint.h>
#include <stddef.h>

// External symbols from the linker script defining the heap region.
extern unsigned int _heap_size;
extern unsigned int _heap_start;

static size_t heap_size;
static void *heap_start;

// Number of pages in the heap region.
static size_t heap_pages;

// Starting address of the allocatable region.
static void *alloc_start;

// Starting page index of allocations (relative to the heap start).
static size_t alloc_start_page_idx;

// Number of pages in the allocatable region.
static size_t alloc_pages;

/* Align an address at a given power of two. */
void *align(void *addr, unsigned int power_of_two)
{
    return (void *) (((phys_addr_t)addr + power_of_two - 1) & ~(power_of_two - 1));
}

/* Initialize the memory management structures.
 * Those structures will reside at the beginning of the heap section
 * and will be followed by the allocatable region.
 */
void init_mm(void)
{
    // Get the values of the external symbols.
    heap_start = &_heap_start;
    heap_size = (size_t) &_heap_size;

    heap_pages = heap_size / PAGE_SIZE;
    struct page *p = heap_start;


    alloc_start = align(heap_start + heap_pages * sizeof(struct page), PAGE_SIZE);
    alloc_start_page_idx = ((phys_addr_t)alloc_start - (phys_addr_t)heap_start) / PAGE_SIZE;
    alloc_pages = heap_pages - alloc_start_page_idx;


    // The first pages in the heap region store the page structures, so they are not allocatable.
    for (size_t i = 0; i < alloc_start_page_idx; i++) {
        p[i].free = 0;
        p[i].last = 0;
    }

    for (size_t i = alloc_start_page_idx; i < heap_pages; i++) {
        p[i].free = 1;
        p[i].last = 0;
    }
}

/* Allocate a number of contiguous memory pages. */
void *page_alloc(unsigned int pages_to_alloc)
{
    struct page *p = heap_start;
    void *addr;
    int start_page_idx = -1;
    size_t i, j;

    // Find a contiguous sequence of free memory pages.
    for (i = alloc_start_page_idx; i < heap_pages; i++) {
        if (p[i].free) {
            size_t nr = 1;
            for (j = i + 1; nr < pages_to_alloc && j < heap_pages; nr++, j++) {
                if (!p[j].free)
                    break;
            }
            if (nr < pages_to_alloc) {
                i = j;
            }
            else {
                addr = (uint8_t *) heap_start + i * PAGE_SIZE;
                start_page_idx = i;
                break;
            }
        }
    }

    if (start_page_idx == -1)
        return NULL;

    p = heap_start;
    for (i = start_page_idx; i < start_page_idx + pages_to_alloc; i++) {
        p[i].free = 0; // Mark the page as allocated.
        if (i == start_page_idx + pages_to_alloc - 1)
            p[i].last = 1;
    }


    return addr;
}

/* Free a previously allocated sequence of memory pages
 * starting at a given address.
 */
void page_free(void *addr)
{
    size_t addr_page_idx = ((phys_addr_t)addr - (phys_addr_t)heap_start) / PAGE_SIZE;
    struct page *p = heap_start;
    size_t i = addr_page_idx;

    // TODO: Verify that the pages have been previously allocated.
    while (i < heap_pages && !p[i].last)
        p[i++].free = 1;

    if (i < heap_pages) {
        p[i].free = 1;
        p[i].last = 0;
    }
}





