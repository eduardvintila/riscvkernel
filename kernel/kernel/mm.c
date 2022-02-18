/* Memory Management */

#include <kernel/mm.h>
#include <kernel/types.h>

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

// Initial metadata slab used for storing the slab structs.
static struct slab slab_metadata;

// List of all metadata slabs.
static struct slab *metadata_slabs;

// List of all non-metadata slabs.
static struct slab *slabs;


/* Align an address at a given power of two. */
void *align(void *addr, unsigned int power_of_two)
{
    return (void *) (((physical_addr_t)addr + power_of_two - 1) & ~(power_of_two - 1));
}


/*
 * Initialize a slab region for objects of a fixed size.
 */
static void init_slab(struct slab *s, unsigned int size) {
    struct free_slab_obj *obj;

    // The minimum number of memory pages needed to represent the slab.
    unsigned int pages = size / PAGE_SIZE + 1;

    // The number of objects (entries) in the slab.
    unsigned int entries;

    s->size = size;
    s->freelist_head = page_alloc(pages); // TODO: Check if allocation was successful
    s->start = (physical_addr_t) s->freelist_head;
    s->next_slab = NULL;

    entries = (pages * PAGE_SIZE) / size;
    obj = s->freelist_head;
    // Each free object in the slab represents a free_slab_obj struct which points to the next free object.
    for (unsigned int i = 1; i < entries; i++) {
        obj->next = (struct free_slab_obj *) (s->start + i * s->size);
        obj = obj->next;
    }
    // The last object in the slab doesn't have a next free object to point to.
    obj->next = NULL;
}

/*
 * Initialize the memory management structures.
 */
void init_mm(void)
{
    // Get the values of the external symbols, describing the starting address of the heap and it's size.
    heap_start = &_heap_start;
    heap_size = (size_t) &_heap_size;

    heap_pages = heap_size / PAGE_SIZE;
    struct page *p = heap_start;


    alloc_start = align(heap_start + heap_pages * sizeof(struct page), PAGE_SIZE);
    alloc_start_page_idx = ((physical_addr_t)alloc_start - (physical_addr_t)heap_start) / PAGE_SIZE;
    alloc_pages = heap_pages - alloc_start_page_idx;


    // The beginning of the heap section will store the bitmap array
    //   with information about all memory pages in the heap.

    // The first pages in the heap region store the page structures, so they are not allocatable.
    for (size_t i = 0; i < alloc_start_page_idx; i++) {
        p[i].free = 0;
        p[i].last = 0;
    }

    // Mark the remaining pages as allocatable.
    for (size_t i = alloc_start_page_idx; i < heap_pages; i++) {
        p[i].free = 1;
        p[i].last = 0;
    }


    // Initialize the first metadata slab which will store the slab structures.
    init_slab(&slab_metadata, sizeof(struct slab));
    metadata_slabs = &slab_metadata;

    // No slabs for allocating memory at the moment.
    slabs = NULL;

    // Preallocate slabs
    // alloc_slab(4);
    // alloc_slab(8);
    // alloc_slab(16);
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
    size_t addr_page_idx = ((physical_addr_t)addr - (physical_addr_t)heap_start) / PAGE_SIZE;
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

/*
 * Allocate a slab for storing objects of a fixed size.
 */
static struct slab *alloc_slab(unsigned int size)
{
    struct slab *s = NULL;
    if (metadata_slabs->freelist_head != NULL) {
        if (metadata_slabs->freelist_head->next == NULL) {
            // This metadata slab has room for one more slab structure.
            // Use this slot to store a new metadata slab in order to represent future slab structures.

            struct slab *meta_slab = metadata_slabs->freelist_head;
            init_slab(meta_slab, sizeof(struct slab));
            meta_slab->next_slab = metadata_slabs;
            metadata_slabs = meta_slab;
        }


        // Store the structure which describes the slab as an object in the current metadata slab.
        s = (struct slab *) metadata_slabs->freelist_head;
        metadata_slabs->freelist_head = metadata_slabs->freelist_head->next;

        init_slab(s, size);

        // Add it at the top of the list of slabs.
        s->next_slab = slabs;
        slabs = s;

    }

    return s;
}

/*
 * Allocate space for an object in a given slab.
 */
static void *alloc_slab_obj(struct slab *s)
{
    void *addr = NULL;

    if (s->freelist_head == NULL)
        return NULL;

    // Move the head of the free list to the next free object.
    addr = (void *) s->freelist_head;
    s->freelist_head = s->freelist_head->next;

    return addr;
}

/*
 * Free the space occupied by a specific object in a given slab.
 */
static void free_slab_obj(struct slab *s, struct free_slab_obj *obj)
{
    struct free_slab_obj *p = s->freelist_head;

    if (p != NULL) {
        // If the freelist_head points to a higher address than the object
        if ((physical_addr_t) p > (physical_addr_t) obj) {
            obj->next = p;
            s->freelist_head = obj;
        } else {
            while (p->next != NULL && (physical_addr_t) p->next < (physical_addr_t) obj) { // TODO: Binary search?
                p = p->next;
            }
            // Insert the address of the freed object in the linked list of free objects.
            obj->next = p->next;
            p->next = obj;
        }
    } else {
        obj->next = NULL;
        s->freelist_head = obj;
    }
}

/**
 * Allocate space for a given number of contiguous memory bytes.
 */
void *kalloc(unsigned int bytes)
{
    struct slab *s, *closest = NULL;
    unsigned int closest_size = 0xffffffffU; // UINT_MAX
    int rem;

    if (slabs != NULL) {
        // Find the minimum sized slab which can fit the number of bytes to be allocated.
        s = slabs;
        while (s != NULL) {
            // TODO: Binary search
            // TODO2: Check that the closest slab isn't too big so we don't waste memory
            if (s->size >= bytes && s->size < closest_size && s->freelist_head != NULL) {
                closest = s;
                closest_size = s->size;
            }
            s = s->next_slab;
        }

        if (closest != NULL) {
            return alloc_slab_obj(closest);
        }
    }

    // No appropriate slab was found, therefore a new one has to be created.
    rem = bytes % 8;
    if (bytes % 8 == 0)
        s = alloc_slab(bytes);
    else
        // Align at 8 bytes.
        // TODO: Use align function.
        s = alloc_slab(bytes + (8 - rem));
    return alloc_slab_obj(s);
}

/*
 * Free the space previously allocated at a given address.
 */
void kfree(void *addr)
{
    struct slab *s;
    physical_addr_t _addr = (physical_addr_t) addr;

    if (slabs != NULL) {
        // Find the slab which contains the object allocated at the address.
        s = slabs;
        while (s != NULL) {
            unsigned int slab_len = (s->size / PAGE_SIZE + 1) * PAGE_SIZE; // Maybe define a macro for this expression?
            if (s->start <= _addr && s->start + slab_len > _addr) {
                //int obj_idx = (_addr - s->start) / s->size;
                free_slab_obj(s, (struct free_slab_obj *) addr);
                break;
            }
            s = s->next_slab;
        }
    } // else error!
}





