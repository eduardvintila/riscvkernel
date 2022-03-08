#include <kernel/traps.h>
#include <kernel/utils/kprintf.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>


// Use the "interrupt" attribute so that the compiler can generate entry and exit sequences that save and restore register states.
void trap_handler(void) __attribute__ ((interrupt ("supervisor")));
void trap_handler(void)
{
    kprintf("scause: 0x%x\n", csr_read(CSR_SCAUSE));
}

void init_traps()
{
    physical_addr_t addr = (physical_addr_t) &trap_handler;
    kprintf("sscratch: 0x%x\n", csr_read(CSR_SSCRATCH));
    kprintf("stvec: 0x%x\n", csr_read(CSR_STVEC));
    kprintf("sip: 0x%x\n", csr_read(CSR_SIP));
    kprintf("sie: 0x%x\n", csr_read(CSR_SIE));
    kprintf("sstatus: 0x%x\n", csr_read(CSR_SSTATUS));
    csr_write(CSR_STVEC, addr);
    kprintf("stvec: 0x%x\n", csr_read(CSR_STVEC));
    csr_set(CSR_SIE, SIP_STIP); // Enable supervisor timer interrupt
    csr_set(CSR_SSTATUS, SSTATUS_SIE); // Enable interrupts
    kprintf("sie after enabling interrupts: 0x%x\n", csr_read(CSR_SIE));
    kprintf("sstatus after enabling interrupts: 0x%x\n", csr_read(CSR_SSTATUS));
    kprintf("sip after enabling interrupts: 0x%x\n", csr_read(CSR_SIP));

    // Timer interrupt doesn't work, probably need to activate it via the interrupt controller.

    csr_write(CSR_MSTATUS, 0); // should generate an exception
}
