#include "CppKern.h"
#include "Kernel.h"
#include "BUILD.b"
#include "../lib/Vector/vector.h"
#include "../lib/VirtualConsole/VirtualConsole.h"
#include "../cpu/cpu.h"
#include "../lib/mem/mem.h"
#include "../lib/core/core.h"
#include "../cpu/pic.h"
#include "../lib/hal/hal.h"
#include "../lib/VGA/VGA.h"
#include "../lib/IO/ATA/ata.h"
#include "../lib/PCI/PCI.h"
#include "../lib/IO/Serial/serial.h"
#include "../lib/String/String.h"

extern uint32_t KernelEnd;

multiboot_info_t* mulboot;
uint32 magicinfo = 0;


 
class KernelEntry {
public:
    Serial s;
    
    KernelEntry() {
        SetMemory(mulboot->mem_upper);


        VGA::INIT_DISPLAY();
        VGA::CURSOR::ENABLE(1 , 10);
        

        VGA::SET_COLOR(VGA::COLORS::YELLOW, VGA::COLORS::BLACK);
        VGA::PRINT_STR("VGA info --> [addr: ");
        VGA::PRINT_INT(mulboot->framebuffer_addr);
        VGA::PRINT_STR(", w: ");
        VGA::PRINT_INT(mulboot->framebuffer_width);
        VGA::PRINT_STR(", h: ");
        VGA::PRINT_INT(mulboot->framebuffer_height);
        VGA::PRINT_STR(", type: ");
        VGA::PRINT_INT(mulboot->framebuffer_type);
        VGA::PRINT_STR("] \n");
        
        

        VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
        isr_install();
        irq_install();
        idt_install();
        gdt_install();
        pic_init();
        memoryInit(20 * 1024 * 10);  
        PCI::pci_init();

        
        
        //enable the interrupts
        CPU::enable_intr();
        
        // display useful info about hardware
        VGA::kprintf("Hardware: %d MB, %dx%d VGA, %d Flags \n", Getmemory() / 1024, mulboot->framebuffer_width, mulboot->framebuffer_height,   mulboot->flags);

        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);

        // welcome !
        VGA::kprintf("\nFluxed OS ====== BUILD %d ====== Memory : %d MB -- KRN END: %d KB \n", BUILD, Getmemory() / 1024, KernelEnd);
    
    }

    ~KernelEntry() {
        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);

        VGA::PRINT_STR("\nKernel Exitting...");
    }
   

    void Test() {
        G_COLOR(VGA::PRINT_STR("TESTING Started\n"), VGA::COLORS::CYAN, VGA::COLORS::BLACK);
        VGA::SET_COLOR(VGA::COLORS::MAGENTA, VGA::COLORS::BLACK);

        string str;
        str = "PASSED";

        VGA::kprintf("TESTING VGA :: PF\eAA\eSI\eSL\eEE\eDD\e! , %s , %s", "PASSED", "PASSED!");
        
        

        //VGA::PRINT_STR(R"()");

        //cout << "test" << "\n";

        VGA::PRINT_STR("\nTESTING VECTOR ");
        
        Vector<char> test;

        for (int i = 0; i < 5; i++) {
            test.push_back((char)i + 'a');
        }

        for (int i = 0; i < 5; i++) {
            test[i] = ((char)i + '0');
        }

        // h e l l o
        // 0 1 2 3 4

        test[3] = 'l';

        test.pop_at(0);
        test.insert_at(0, 'h');

        test.pop_back();
        test.push_back('o');


        test.pop_at(1);
        test.insert_at(1, 'e');

        test[2] = 'l';

        

        const char * testingFinal = "hello";
        for (int i = 0; i < test.size(); i ++) {
            if (test[i] == testingFinal[i]) continue;
            else G_ERR(VGA::PRINT_CHAR(test[i]));
        }

        G_OK;

        char * mem_test = (char*)kmalloc(MEMORY::PAGE);
        char * mem_test1 = (char*)kmalloc(MEMORY::PAGE);
        char * mem_test2 = (char*)kmalloc(MEMORY::PAGE);
        char * mem_test3 = (char*)kmalloc(MEMORY::PAGE);

        //kfree(mem_test);
        //kfree(mem_test);


        //ThrowISR(19);
        KDEBUG;
    }

    void kern() {
        G_COLOR(VGA::PRINT_STR("Kernel Successfully Booted\n"), VGA::COLORS::CYAN, VGA::COLORS::BLACK);
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
