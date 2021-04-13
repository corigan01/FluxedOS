#ifdef __cplusplus  
extern "C" { 
#endif 
    void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
    void idt_install();
#ifdef __cplusplus 
} 
#endif 

