#ifndef CPU__LIB__H
#define CPU__LIB__H

#include "../core/core.h"
#include "../../cpu/cpu.h"
#include "../VGA/VGA.h"

#define PUSH_A \ 
        asm volatile("pusha")

#define POP_A \
    asm volatile("popa")

#define NO_INSTRUCTION \
    asm volatile("nop")

#define HALT \ 
    while(1) { NO_INSTRUCTION; } 

namespace CPU
{
    namespace GDT
    {
        
    } // namespace GDT
    
    namespace IDT
    {
        
    } // namespace IDT
    
    namespace IRQ
    {
        #define START \
            asm volatile("add $0x1c, %esp"); \
            asm volatile("pusha");

        #define END \
            asm volatile("popa"); \
	        asm volatile("iret");

        void send_irq(uint8 irq);


    } // namespace IRQ
    
    namespace ISR
    {
        
    } // namespace ISR

    
    
} // namespace CPU


#define panic \
    VGA::CLEAR_DISPLAY(); \
    VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK); \
    VGA::PRINT_STR("==KERNEL PANIC CALLED==  at ["); \
    VGA::PRINT_STR(__FILE__); \
    VGA::PRINT_STR(": "); \ 
    VGA::PRINT_INT(__LINE__); \ 
    VGA::PRINT_STR("], ARG{"); \
    VGA::PRINT_STR(""); \
    VGA::PRINT_STR("} Halting CPU!\n"); \
    HALT; 



#endif // !CPU__LIB__H
