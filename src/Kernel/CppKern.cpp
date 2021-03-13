#include "CppKern.h"
#include "Kernel.h"
#include "BUILD.h"
#include "../lib/Vector/vector.h"
#include "../lib/VirtualConsole/VirtualConsole.h"
#include "../cpu/cpu.h"
#include "../lib/mem/mem.h"
#include "../lib/core/core.h"
#include "../lib/IO/ide.h"
#include "../cpu/pic.h"
#include "../lib/hal/hal.h"
#include "../lib/VGA/VGA.h"
#include "../lib/IO/ide.h"

extern uint32_t end;

class KernelEntry {
public:
    
    KernelEntry() {
        VGA::INIT_DISPLAY();
        VGA::CURSOR::ENABLE(1 , 10);

        VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);


        isr_install();
        irq_install();
        idt_install();
        gdt_install();
        pic_init();
        memoryInit(end);
        
        

        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);


        VGA::PRINT_STR("\nFluxed OS ====== BUILD ");
        VGA::PRINT_INT(BUILD);
        VGA::PRINT_STR(" ====== Memory : ");
        VGA::PRINT_INT(Getmemory());
        VGA::PRINT_STR("KB ");
        VGA::PRINT_STR(" -- KRN END: ");
        VGA::PRINT_INT(end / 1024);
        VGA::PRINT_STR("KB \n");
        
        //enable the interrupts
        Vasm("sti");
       //pic_send(10);
    }

    ~KernelEntry() {
        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);

        VGA::PRINT_STR("\nKernel Exitting...");
    }
   

    void Test() {
        VGA::SET_COLOR(VGA::COLORS::MAGENTA, VGA::COLORS::BLACK);
        VGA::PRINT_STR("TEST --- ");
        VGA::PRINT_INT(10);

        VGA::PRINT_STR("\nThis should be a new line! PF\eAA\eSI\eSL\eEE\eDD\e!");
        VGA::PRINT_STR(R"(
FluxedOS String Test

New line!
        )");

        VGA::PRINT_STR("\n");
        
        uint32_t* target;
        ATA::read_sectors_ATA_PIO(target, 0x0, 1);

        for (int i = 0; i < 128; i++) {
            VGA::PRINT_INT(target[i] && 0xFF);
            VGA::PRINT_STR(" : ");
            VGA::PRINT_INT((target[i] >> 8) & 0xFF);

            if (i % 10 == 0) {
                VGA::PRINT_STR("\n");
            }
        }

        //ThrowISR(19);
    }

    void kern() {
        VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
        VirtualConsole console;

        console.Handle();
        
        
        //VGA::PRINT_STR(">\n\n\n\n\n\n>\n>\nline\n>test\n>wut\nthis is a lot of printing on this line oh yea");

        

    }
};

int KernStart() {
    {
        KernelEntry krnl;

        krnl.Test();
        krnl.kern();
    }

    panic("Kernel should not quit!");
}
