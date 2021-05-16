;/*
; *       ______            __ __                 __
; *      / __/ /_ ____ __  / //_/__ _______  ___ / /
; *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
; *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
; *    
; *   copyright (c) 2021 Gavin Kellam (aka corigan01)
; *   
; *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
; *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
; *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
; *
; *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
; *
; *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
; *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
; *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
; *  
; *   
; */

.code32
.section .text
.extern long_mode_start
.global _start
_start:
    # setup stack using it's physical address (in higher half kernel we need to use phys addresses until we are in long mode with paging enabled)
    mov $phys_kernel_stack_top, %esp

    # store multiboot2 info struct pointer in edi so it then gets passed to kmain function
    mov %ebx, %edi

    # check if kernel was actually loaded by the bootloader
    call check_multiboot

    # check if CPUID is supported
    call check_cpuid

    # check if long mode is supported
    call check_long_mode

    # checkups done, print hello message
    mov $phys_HELLO_MSG, %esi
    call print

    # setup and enable paging
    call set_up_page_tables
    call enable_paging

    # load global descriptor table that contains our only one 64bit code selector
    lgdt phys_gdt64_struct_pointer

    # load cs with gdt64_code_offset by doing far return (retf) to $phys_long_mode_start
    push $gdt64_code_offset             # push new value of code segment offset in GDT (CS)
    push $phys_long_mode_start          # push return address, use physical address as we are still in 32 bit mode
    retf                                # pop EIP, pop CS; effectively far jumping to $phys_long_mode_start


/**
    print
    Put a string to the screen.
    %esi - null terminated string address
*/
print:
    # %edi = vga text buffer address, save it and restore upon return
    push %edi
    mov $0xb8000, %edi

    .loop:
        # fetch one character
        movb (%esi), %dl

        # check if \0
        cmp $0, %dl
        je .end_loop

        # copy characterto vga textbuffer
        movb %dl, (%edi)
        movb $0x4f, 1(%edi) # 0x4f is white text on red background

        # advance the source pointer and textbuffer pointer
        add $1, %esi
        add $2, %edi
        jmp .loop

    .end_loop:
        pop %edi
        ret

/**
    check_multiboot
    Check if bootloader actually properly loaded our kernel
*/
check_multiboot:
    cmp $0x36d76289, %eax   # check for magic number in eax
    jne .no_multiboot
    ret

    .no_multiboot:
        mov $phys_ERROR_NO_KERNEL_LOADED, %esi
        call print
        hlt

/**
    check_cpuid
    Check if CPUID instruction is supported
*/
check_cpuid:
    # Check if CPUID is supported by attempting to flip the ID bit (bit 21)
    # in the FLAGS register. If we can flip it, CPUID is available.

    # Copy FLAGS in to EAX via stack
    pushf
    pop %eax

    # Copy to ECX as well for comparing later on
    mov %eax, %ecx

    # Flip the ID bit, 0x200000 = 1 << 21
    xorl $0x200000, %eax

    # Copy EAX to FLAGS via the stack
    push %eax
    popf

    # Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pushf
    pop %eax

    # Restore FLAGS from the old veesion stored in ECX (i.e. flipping the
    # ID bit back if it was ever flipped).
    push %ecx
    popf

    # Compare EAX and ECX. If they are equal then that means the bit
    # wasn't flipped, and CPUID isn't supported.
    cmpl %eax, %ecx
    je .no_cpuid
    ret

    .no_cpuid:
        mov $phys_ERROR_NO_CPUID, %esi
        call print
        hlt


/**
    check_long_mode
    Check if longmode is supported
*/
check_long_mode:
    # test if extended processor info in available
    mov $0x80000000, %eax   # implicit argument for cpuid
    cpuid                   # get highest supported argument
    cmpl $0x80000001, %eax  # it needs to be at least 0x80000001
    jb .no_long_mode        # if it's less, the CPU is too old for long mode

    # use extended info to test if long mode is available
    mov $0x80000001, %eax   # argument for extended processor info
    cpuid                   # returns various feature bits in ecx and edx
    test $0x20000000, %edx  # test if the LM-bit is set in the D-register, 0x20000000 = 2 << 29
    jz .no_long_mode        # If it's not set, there is no long mode
    ret

    .no_long_mode:
        mov $phys_ERROR_NO_LONGMODE, %esi
        call print
        hlt

/**
    set_up_page_tables
    Setup page tables to map first 1GiB and -2GiB(higher half kernel) of 64 bit virtual address space to first GiB of RAM before going into long mode.
    Use huge pages (2MiB); so we only need to configure P4 (1 entry), P3 (1 entry), P2 (512 entries) page tables.
    P2 maps directly to a 2MiB page that is indexed with 21 remaining bits of the address, see:
    http://wiki.osdev.org/Page_Tables#2_MiB_pages_2
    This is identity mapping; virtual addresses map to the same physical addresses
*/
set_up_page_tables:
    # map first P4 entry to P3 table
    mov $phys_p3_table, %eax
    or $0b111, %eax                      # present + writable + user space accessible
    mov %eax, (phys_p4_table + 0 * 8)    # map first huge 512 GB block of virt mem - p4 entry no. 0
    mov %eax, (phys_p4_table + 511 * 8)  # map last huge 512 GB block of virt mem - p4 entry no. 511

    # map first P3 entry to P2 table
    mov $phys_p2_table, %eax
    or $0b111, %eax                      # present + writable + user space accessible
    mov %eax, (phys_p3_table + 0 * 8)    # map 1-st GB of virt mem - p3 entry no. 0
    mov %eax, (phys_p3_table +  510 * 8) # map 511-th GB of virt mem - p3 entry no. 510

    # map each P2 entry to a huge 2MiB page
    mov $0, %ecx

    .map_p2_table:
        # map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
        mov $0x200000, %eax     # 2MiB
        mul %ecx                # start address of ecx-th page
        or $0b10000111, %eax    # present + writable + user space accessible + huge
        mov %eax, phys_p2_table(, %ecx, 8) # map ecx-th entry; memory[phys_p2_table + ecx * 8] = eax

        inc %ecx                # increase counter
        cmp $512, %ecx          # if counter == 512, the whole P2 table is mapped
        jne .map_p2_table       # else map the next entry

        ret

/**
    enable_paging
    Enable PAE, long mode and paging
*/
enable_paging:
    # load P4 to cr3 register (cpu uses this to access the P4 table)
    mov $phys_p4_table, %eax
    mov %eax, %cr3

    # enable PAE-flag in cr4 (Physical Address Extension)
    mov %cr4, %eax
    or $32, %eax  # 1 << 5
    mov %eax, %cr4

    # set the long mode bit in the EFER MSR (model specific register)
    mov $0xC0000080, %ecx
    rdmsr
    or $256, %eax # 1 << 8
    wrmsr

    # enable paging in the cr0 register
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    ret


# zero-initialized data section
.section .bss

# make sure each page table fit into one memory page
.align 4096

# 4 level page tables for long mode, each has 512 entries of 8 byte pointer
.lcomm p4_table, 4096
.lcomm p3_table, 4096
.lcomm p2_table, 4096

# create stack memory
kernel_stack_bottom:
    .lcomm STACK_DATA, 4*4096
kernel_stack_top:
.global kernel_stack_top

.section .rodata
KERNEL_VIRTUAL_BASE:
    .quad KVIRT_BASE
.global KERNEL_VIRTUAL_BASE

# Global Descriptor Table for 64bit longmode
gdt64:
.equ phys_gdt64, gdt64 - KVIRT_BASE
    .quad 0 # start with zero-entry
gdt64_code:
    .quad 0x20980000000000 # then code segment entry; USER_SEGMENT(44) | PRESENT(47) | EXECUTABLE(43) | LONG_MODE(53)
gdt64_struct_pointer:
    .word gdt64_struct_pointer - gdt64 - 1  # GDT length -1
    .quad phys_gdt64                        # GDT address
.equ gdt64_code_offset, gdt64_code - gdt64  # GDT code segment offset in phys_gdt64 struct
.global gdt64_code_offset




# Printable messages
HELLO_MSG:              .asciz "Protected mode (32bit)"
ERROR_NO_KERNEL_LOADED: .asciz "Kernel not properly loaded by GRUB"
ERROR_NO_CPUID:         .asciz "CPUID not supported"
ERROR_NO_LONGMODE:      .asciz "Long mode not supported"



/**
 Higher Half kernel related stuff
*/


# Kernel virtual base; we use -2GB in 64bit address space, so 32bit signed relative addressing can reach the kernel data
.equ KVIRT_BASE,                    0xFFFFFFFF80000000

.equ phys_p4_table,                 p4_table - KVIRT_BASE
.equ phys_p3_table,                 p3_table - KVIRT_BASE
.equ phys_p2_table,                 p2_table - KVIRT_BASE

.equ phys_HELLO_MSG,                HELLO_MSG - KVIRT_BASE
.equ phys_ERROR_NO_KERNEL_LOADED,   ERROR_NO_KERNEL_LOADED - KVIRT_BASE
.equ phys_ERROR_NO_CPUID,           ERROR_NO_CPUID - KVIRT_BASE
.equ phys_ERROR_NO_LONGMODE,        ERROR_NO_LONGMODE - KVIRT_BASE

.equ phys_kernel_stack_top,         kernel_stack_top - KVIRT_BASE
.equ phys_gdt64_struct_pointer,     gdt64_struct_pointer - KVIRT_BASE
.equ phys_long_mode_start,          long_mode_start - KVIRT_BASE