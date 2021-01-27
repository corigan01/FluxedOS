#ifdef __cplusplus  
extern "C" { 
#endif 
    void isr_install();
    //the following is a data structure that defines how stack looks when an ISR is running
    //as we know, we push the pointer of stack on to the stack when an ISR occurs, to handle those interrutps using our
    //C fault handler we use the following to show how stack looks like.
    struct regs
    {
        unsigned int gs, fs, es, ds;      /* pushed the segs last */
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
        unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
        unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
    };
#ifdef __cplusplus 
} 
#endif 
