OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
	ram (AWX) : ORIGIN = 0x80200000, LENGTH = 512M
}

/*
TODO: Manually specify the program headers and their specific permissions (R, W or E)
PHDRS
{
	text PT_LOAD;
	rodata PT_LOAD;
	data bss PT_LOAD;

} */

SECTIONS
{
	. = 0x80200000; /* Physical address at which OpenSBI jumps */
	_kernel_start = .;

	.text ALIGN (4K) : AT (ADDR (.text))
	{
		*(.entry)
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata))
	{
		*(.rodata .rodata.*)
	}
	.data ALIGN (4K) : AT (ADDR (.data))
	{
		*(.data)
		*(.data.*)
		*(.readmostly.data)
		*(*.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss))
	{
		*(.bss)
		*(.bss.*)
	}

	/*
	 * Allocate the stack right after the .bss section.
	 * Note that the stack grows downwards from _stack_top to _stack_bottom.
	 * TODO: Warning! Make sure the stack region doesn't overlap overlap other memory regions (which could contain the FDT, for example)
	 */
	_stack_bottom = .;
	_stack_top = _stack_bottom + 512K;

	/*
	 * Allocate the heap right after the stack.
	 */
	_heap_start = _stack_top;
	_heap_end = ORIGIN(ram) + LENGTH(ram);
	_heap_size = _heap_end - _heap_start + 1;
}
