#ifndef CPU__LIB__H
#define CPU__LIB__H

#include "../core/core.h"
#include "../../cpu/cpu.h"
#include "../VGA/VGA.h"
#include "../IO/port.h"



namespace SYS
{
    void reset();
    void shutdown();
} // namespace SYS


namespace CPU
{
    void enable_intr();
    void disable_intr();
    namespace GDT
    {
        
    } // namespace GDT
    
    namespace IDT
    {
        
    } // namespace IDT
    
    namespace IRQ
    {
        void START();

        void END();

        void send_irq(uint8 irq);
        

    } // namespace IRQ
    
    namespace ISR
    {
        
    } // namespace ISR

    
    
} // namespace CPU



#endif // !CPU__LIB__H
