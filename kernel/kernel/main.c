#include <stdio.h>
#include <kernel/sbi.h>
#include <kernel/mm.h>
#include <stdint.h>

void *debug_page_alloc(unsigned int pages)
{
    void *addr = page_alloc(pages);
    printf("Debug alloc %u pages: %l - %l\n", pages, addr, (phys_addr_t)addr + PAGE_SIZE * pages);
    return addr;
}

void debug_page_free(void *addr)
{
    printf("Free %l\n", (phys_addr_t) addr);
    page_free(addr);
}

void *debug_kalloc(unsigned int bytes)
{
    void *addr = kalloc(bytes);
    printf("Debug alloc %u bytes: %l - %l\n", bytes, addr, (phys_addr_t)addr + bytes);
    return addr;
}

void *debug_kfree(void *addr)
{
    printf("Free %l\n", (phys_addr_t) addr);
    kfree(addr);
}

#define ALLOCS 10


void test_alloc1(void)
{
    void *addrs[ALLOCS];
    void *addr1;
    void *addr2;
    void *addr3;
    void *addr4;

    printf("Testing page allocations...\n");
    addr1 = debug_page_alloc(4);
    addr2 = debug_page_alloc(2);
    addr3 = debug_page_alloc(1);
    debug_page_free(addr1);
    addr1 = debug_page_alloc(3);
    addr4 = debug_page_alloc(2);
    debug_page_free(addr3);
    addr3 = debug_page_alloc(1);
    debug_page_alloc(1);

    printf("Testing byte allocations...\n");
    for (int i = 0; i < ALLOCS; i++)
        addrs[i] = debug_kalloc(32);

    debug_kfree(addrs[5]);
    debug_kfree(addrs[1]);
    addrs[1] = debug_kalloc(32);
    addrs[5] = debug_kalloc(32);

    printf("\n\n\n");
    for (int i = 0; i < ALLOCS; i++)
        addrs[i] = debug_kalloc(7);

    debug_kfree(addrs[5]);
    debug_kfree(addrs[7]);
    addrs[0] = debug_kalloc(7);
    addrs[7] = debug_kalloc(7);
    debug_kalloc(7);


    // addr1 = debug_kalloc(8);
    // addr2 = debug_kalloc(32);
    // addr3 = debug_kalloc(4);
    // addr4 = debug_kalloc(8);
    // debug_kalloc(5192);

    // printf("sizeof(size_t) = %l\n", sizeof(size_t));

    // debug_page_free(addr2);
    // debug_page_free(addr3);
}

void test_alloc2(void)
{
    void *addrs[500];

    printf("Testing kalloc allocations...\n");
    for (int i = 0; i < 500; i++) {
        addrs[i] = debug_kalloc(8);
    }

    for (int i = 250; i < 400; i += 2) {
        debug_kfree(addrs[i]);
    }

    for (int i = 250; i < 400; i++) {
        addrs[i] = debug_kalloc(8);
    }

    for (int i = 0; i < 100; i++) {
        debug_kalloc(29);
    }
}

void kernel_main(void)
{

    printf("Entering the main kernel!\n");
    struct sbiret ret = sbi_get_spec_version();
    printf("SBI version: %l.%l\n", ret.error, ret.value);
    init_mm();
    // TODO: Should actually verify that the mm module init was successful.
    printf("Memory management module successfully initialized!\n");
    test_alloc2();
}
