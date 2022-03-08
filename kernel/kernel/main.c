#include <kernel/sbi_ecalls.h>
#include <kernel/mm.h>
#include <kernel/types.h>
#include <kernel/traps.h>
#include <kernel/utils/kprintf.h>

void *debug_page_alloc(unsigned int pages)
{
    void *addr = page_alloc(pages);
    kprintf("Debug alloc %u pages: 0x%x - 0x%x\n", pages, addr, (physical_addr_t)addr + PAGE_SIZE * pages);
    return addr;
}

void debug_page_free(void *addr)
{
    kprintf("Free 0x%x\n", (physical_addr_t) addr);
    page_free(addr);
}

void *debug_kalloc(unsigned int bytes)
{
    void *addr = kalloc(bytes);
    kprintf("Debug alloc %u bytes: 0x%x - 0x%x\n", bytes, addr, (physical_addr_t)addr + bytes);
    return addr;
}

void *debug_kfree(void *addr)
{
    kprintf("Free 0x%x\n", (physical_addr_t) addr);
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

    kprintf("Testing page allocations...\n");
    addr1 = debug_page_alloc(4);
    addr2 = debug_page_alloc(2);
    addr3 = debug_page_alloc(1);
    debug_page_free(addr1);
    addr1 = debug_page_alloc(3);
    addr4 = debug_page_alloc(2);
    debug_page_free(addr3);
    addr3 = debug_page_alloc(1);
    debug_page_alloc(1);

    kprintf("Testing byte allocations...\n");
    for (int i = 0; i < ALLOCS; i++)
        addrs[i] = debug_kalloc(32);

    debug_kfree(addrs[5]);
    debug_kfree(addrs[1]);
    addrs[1] = debug_kalloc(32);
    addrs[5] = debug_kalloc(32);

    kprintf("\n\n\n");
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

    // kprintf("sizeof(size_t) = 0x%x\n", sizeof(size_t));

    // debug_page_free(addr2);
    // debug_page_free(addr3);
}

void test_alloc2(void)
{
    void *addrs[500];

    kprintf("Testing kalloc allocations...\n");
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

    kprintf("Entering the main kernel!\n");
    struct sbiret ret = sbi_get_spec_version();
    kprintf("SBI version: %ld.%ld\n", ret.error, ret.value);
    init_mm();
    // TODO: Should actually verify that the mm module init was successful.
    kprintf("Memory management module successfully initialized!\n");
    //test_alloc2();
    init_traps();
}
