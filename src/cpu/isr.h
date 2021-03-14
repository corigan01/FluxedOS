#ifndef ISR_CPU__H
#define ISR_CPU__H


#ifdef __cplusplus  
extern "C" { 
#endif 
    typedef void (*isr_t)(register_t * );
    void isr_install();

    struct regs
    {
        unsigned int gs, fs, es, ds;      /* pushed the segs last */
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
        unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
        unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
    };

    void register_interrupt_handler(int num, isr_t handler);

    void fault_handler(struct regs *r);
#ifdef __cplusplus 
} 
#endif 


#endif // !ISR_CPU__H
