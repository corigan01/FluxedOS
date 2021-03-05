bits   32

global _gdt_flush
extern gp

_gdt_flush:
    lgdt [gp]       ;special pointer gp to load the GDT
    mov ax, 0x10    ;0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2 ;0x08 is the offset to code segmnet and flush2 is far jump
flush2:
    ret

;lets load the IDT
global load_idt
extern idtp
load_idt:
    lidt [idtp]
    ret


extern boot.s
global load
extern stublet
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
    
    
    jmp stublet

extern isr.s
extern irq.s