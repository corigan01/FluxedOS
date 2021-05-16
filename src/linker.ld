ENTRY(_start)

KERNEL_BASE = 0xFFFFFFFF80000000; /* -2GB in 64bit address space, so 32bit signed relative addressing can reach the kernel data */
SECTIONS {
    . = KERNEL_BASE + 0x00100000; /* Kernel starts at 1MB offset */

    .text : AT(ADDR(.text) - KERNEL_BASE) {
        *(.multiboot_header)
        *(.text .text.*)
    }
    
    .init_array : AT(ADDR(.init_array) - KERNEL_BASE) {
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        end_ctors = .;
    }
    
    .fini_array : AT(ADDR(.fini_array) - KERNEL_BASE) {
        KEEP(*( .fini_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .fini_array.* )));
    } 
        
    .data : AT(ADDR(.data) - KERNEL_BASE) {
        *(.data .data.*)
    }
    
    .rodata : AT(ADDR(.rodata) - KERNEL_BASE) {
        *(.rodata .rodata.*)
    }
        
    .bss : AT(ADDR(.bss) - KERNEL_BASE) {
        *(.bss .bss.*)
    } 
}