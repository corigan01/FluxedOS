#ifdef __cplusplus  
extern "C" { 
#endif 
    void isr_install();

    struct regs
    {
        unsigned int gs, fs, es, ds;      /* pushed the segs last */
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
        unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
        unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
    };

    void fault_handler(struct regs *r);
#ifdef __cplusplus 
} 
#endif 
