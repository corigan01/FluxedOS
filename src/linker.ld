ENTRY(start)
 
SECTIONS
{

    . = 1M;

    . = start;

    .text BLOCK(4K) : ALIGN(4K)
    {
      *(.multiboot)
      *(.text)
    }

    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
      *(.rodata)
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
      *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
      *(COMMON)
      *(.bss)
    }

    end = .;

}

