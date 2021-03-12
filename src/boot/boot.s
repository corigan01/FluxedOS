[BITS 64]
global start

start:
    ;mov esp, _sys_stack     ; This points the stack to our new stack area
    jmp stublet

; This part MUST be 4byte aligned, so we solve that issue using 'ALIGN 4'
ALIGN 4

global stublet

stublet:
    extern kernal_entry
    call kernal_entry
    
    
