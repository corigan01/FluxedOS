[BITS 32]
global start

extern cpu.s
extern load
start:
    mov esp, _sys_stack     ; This points the stack to our new stack area
    jmp stublet


; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
global stublet
extern kernal_entry

stublet:
    push eax
    push ebx
    call kernal_entry
    hlt
    


SECTION .bss
    resb 1024*1024               ; This reserves memory here
_sys_stack: