#include <stdio.h>
#include <kernel/sbi.h>
#include <kernel/mm.h>

void *debug_alloc(unsigned int pages)
{
    void *addr = page_alloc(pages);
    printf("Debug alloc %u pages: %l - %l\n", pages, addr, (unsigned long long)addr + PAGE_SIZE * pages);
    return addr;
}

void debug_free(void *addr)
{
    printf("Free %l\n", (unsigned long long) addr);
    page_free(addr);
}

void kernel_main(void)
{
	void *addr1;
    void *addr2;
    void *addr3;
    void *addr4;
    printf("Entering the main kernel!\n");
    struct sbiret ret = sbi_get_spec_version();
    printf("SBI version: %l.%l\n", ret.error, ret.value);
    init_mm();
    printf("Memory management module successfully initialized!\n");
    printf("Testing page allocations...\n");
    addr1 = debug_alloc(4);
    addr2 = debug_alloc(2);
    addr3 = debug_alloc(1);
    debug_free(addr1);
    addr1 = debug_alloc(3);
    addr4 = debug_alloc(2);
    debug_free(addr3);
    addr3 = debug_alloc(1);
    debug_alloc(1);

    // debug_free(addr2);
    // debug_free(addr3);

}
