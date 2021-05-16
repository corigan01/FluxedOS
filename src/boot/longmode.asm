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


.code64
.section .text
.extern KERNEL_VIRTUAL_BASE
.extern kmain
.global long_mode_start
long_mode_start:
    # load 0 into all data segment registers
    mov $0, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # print hello message to screen
    mov $HELLO_MSG, %rsi
    call print_string

    # enter c++ main routine using its absolute address - we start to execute higher half kernel here
    # %rdi holds multiboot2 information pointer so it's passed as 1st parameter according to SYSTEMV x64 ABI
    add KERNEL_VIRTUAL_BASE, %rsp   # make stack pointer a higher half pointer
    add KERNEL_VIRTUAL_BASE, %rdi   # make multiboot2 info pointer a higher half pointer
    movabs $kmain, %rax             # load absolute address of kmain
    call *%rax                      # jump to higher half kernel
    hlt                             # in case that kmain returns for some reason :)

/**
    print_string
    Put a string to the screen.
    %rsi - null terminated string address
*/
print_string:
    # %rdi = vga text buffer address, save it and restore upon return
    push %rdi

    mov $0xFFFFFFFF800b80A0, %rdi # second line in 80-cols 2-byte character vga text buffer

    .loop:
        # fetch one character
        movb (%rsi), %dl

        # check if \0
        cmp $0, %dl
        je .end_loop

        # copy character to vga textbuffer
        movb %dl, (%rdi)
        movb $0x2f, 1(%rdi) # 0x2f is white text on green background

        # advance the source pointer and textbuffer pointer
        add $1, %rsi
        add $2, %rdi
        jmp .loop

    .end_loop:
        pop %rdi
        ret

 .section .rodata
 HELLO_MSG: .asciz "Hello in PhobOS long mode 64bit!"