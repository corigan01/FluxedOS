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
VMODE    equ  1 << 2            ; # enable video
FLAGS    equ  MBALIGN | MEMINFO | VMODE ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

WIDTH   equ 1024
HEIGHT  equ 768
DEPTH   equ 32
HEADER_ADDR equ 0

LOAD_ADDR equ 0
LOAD_END_ADDR equ 0
BSS_END_ADDR equ 0
ENTRY_ADDR equ 0
MODE_TYPE equ 0

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
    dd HEADER_ADDR
    dd LOAD_ADDR
    dd LOAD_END_ADDR
    dd BSS_END_ADDR
    dd ENTRY_ADDR
    dd MODE_TYPE
    dd WIDTH
    dd HEIGHT
    dd DEPTH
 
global stublet
extern kernal_entry

stublet:
    push eax
    push ebx
    call kernal_entry
    hlt
    


SECTION .bss
    resb 1024*1024               ; This reserves 8KBytes of memory here
_sys_stack: