#include <sbi/riscv_encoding.h>
#include <sbi/riscv_asm.h>

#define BASE_ADDR 0xFFFF800000000000
#define PTE 0x00000000000000EF


#0000.0000.0000.0000.0000.0000.0000.0000.0000.0  1000.0000.0010.0000.0000 00 0010.1111
#0000.0000.0000.0000.0000.0000.0000.0000.0000.0  1111.1101.1110.1100.1 00 0010.1111

.section .bss, "aw", @nobits
	.align 12 # Align at 2^12 = 4K bytes
boot_page_table0:
	.skip 4096 # 2^9 page table entries, 8 bytes each => 4K space needed

.section .entry, "ax", %progbits
.align 3 # Align at 2^3 = 8 bytes
.globl _start

_start:

    # TODO: Choose a boot hart, and stop the other harts.

    # Divide the page table address by 4K to obtain it's physical page number (PPN)
    la s3, boot_page_table0
    srli s3, s3, 12


    xor s2, s2, s2
    li t0, SATP_MODE_SV48
    slli t0, t0, 60
    li t1, SATP64_MODE
    and t1, t1, t0
    or s2, s2, t1 # Set 48-bit virtual addressing
    li t1, SATP64_PPN
    and t1, t1, s3
    or s2, s2, t1 # Set the root page table PPN

    la t0, boot_page_table0
    /*
        Identity-map the first 512Gib addresses (0x000000000000 - 0x007FFFFFFFFF).
        This is necessary because after enabling paging the PC still points to a physical address.
    */
    li s1, PTE
    sd s1, 0(t0)

    #sd s1, 256(t0) # Map 0x800000000000 - 0x807FFFFFFFFF to 0x000000000000 - 0x007FFFFFFFFF

    csrw satp, s2 # Enable paging
    sfence.vma x0, x0 # Flush the MMU cache



    la sp, _stack_top - 8 # Setup the stack
    call kernel_main

_hang: # Hang if the kernel returns
    wfi
    j _hang
