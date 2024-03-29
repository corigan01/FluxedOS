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

;lets load the IDT
global load_idt
extern idtp
load_idt:
    lidt [idtp]
    ret


extern boot.S
global load
extern gp
extern loader
load:
    xor ax, ax
    mov ds, ax

    cli

    mov ax, 0x0010
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
   
    ;jmp 0x08:flush2 ;0x08 is the offset to code segmnet and flush2 is far jump


    lgdt [gp]       ;special pointer gp to load the GDT

   
    lidt [idtp]
    
    
    ;jmp _start

extern isr.S
extern irq.S
extern gdt.S
