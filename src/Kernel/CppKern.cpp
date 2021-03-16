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
#include "../lib/PCI/PCI.h"

extern uint32_t end;

multiboot_info_t* mulboot;
uint32 magicinfo = 0;


class KernelEntry {
public:
    
    KernelEntry() {
        VGA::INIT_DISPLAY();

        VGA::PRINT_STR("MB INFO: ");
        VGA::PRINT_INT((mulboot->mem_lower / 1024) / 1024);
        VGA::PRINT_STR("  \n");

        VGA::CURSOR::ENABLE(1 , 10);

        VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);


        isr_install();
        irq_install();
        idt_install();
        gdt_install();
        pic_init();
        memoryInit(end);
        PCI::pci_init();
        vfs_init();
        ATA::ata_init();

        
        


        //vfs_db_listdir("/");

        //ATA::ata_read_sector();
        //ATA::ide_init(0x1F0, 0x3F6, 0x170, 0x376, 0x000);


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
        VGA::PRINT_STR("TESTING VGA :: PF\eAA\eSI\eSL\eEE\eDD\e!");
        //VGA::PRINT_STR(R"()");

        VGA::PRINT_STR("\n");
        
        

        //ThrowISR(19);
    }

    void kern() {
        VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
        VirtualConsole console;

        console.Handle();
        

    }
};

int KernStart(multiboot_info_t* mbt, uint32 magic) {
    mulboot = mbt;
    magicinfo = magic;
    {
        KernelEntry krnl;

        krnl.Test();
        krnl.kern();
    }

    panic("Kernel should not quit!");
}
