#ifndef __IRQ_H
#define __IRQ_H

#ifdef __cplusplus  
extern "C" { 
#endif 
    void irq_install();
    void irq_install_handler();
    void irq_uninstall_handler();
#ifdef __cplusplus 
} 
#endif 


#endif