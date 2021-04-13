#ifndef __GDT_H
#define __GDT_H
#ifdef __cplusplus  
extern "C" { 
#endif 
    void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
    void gdt_install(); 
#ifdef __cplusplus 
} 
#endif 


#endif