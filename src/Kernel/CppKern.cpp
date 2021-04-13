#include "CppKern.h"
#include "Kernel.h"
#include "BUILD.h"
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
#include "../drive/drive.h"
#include "../lib/IO/Serial/serial.h"
#include "../lib/String/String.h"

extern uint32_t KernelEnd;

multiboot_info_t* mulboot;
uint32 magicinfo = 0;




class KernelEntry {
public:
    Serial s;
    
    KernelEntry() {


        VGA::INIT_DISPLAY();

        VGA::PRINT_STR("MultiBoot: H: ");
        VGA::PRINT_INT(mulboot->mem_upper / 1024);
        VGA::PRINT_STR("MB L: ");
        VGA::PRINT_INT(mulboot->mem_lower / 1024);
        VGA::PRINT_STR("MB --> ");
        VGA::PRINT_STR((char*)mulboot->boot_loader_name);
        VGA::PRINT_STR("  \n");
        SetMemory(mulboot->mem_upper);

        VGA::PRINT_STR("GFX info --> [addr: ");
        VGA::PRINT_INT(mulboot->framebuffer_addr);
        VGA::PRINT_STR(", wd: ");
        VGA::PRINT_INT(mulboot->framebuffer_width);
        VGA::PRINT_STR(", hd: ");
        VGA::PRINT_INT(mulboot->framebuffer_height);
        VGA::PRINT_STR("] \n");

        VGA::CURSOR::ENABLE(1 , 10);

        VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);

        isr_install();
        irq_install();
        idt_install();
        gdt_install();
        pic_init();
        memoryInit(KernelEnd);
        PCI::pci_init();
        vfs_init();
        //ATA::ata_init();
        
        //print_vfstree();
        char* mountPoint = "/";
        ext2_init("/dev/hdd", mountPoint);



        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);


        VGA::PRINT_STR("\nFluxed OS ====== BUILD ");
        VGA::PRINT_INT(BUILD);
        VGA::PRINT_STR(" ====== Memory : ");
        VGA::PRINT_INT(Getmemory());
        VGA::PRINT_STR("KB ");
        VGA::PRINT_STR(" -- KRN END: ");
        VGA::PRINT_INT(KernelEnd / 1024);
        VGA::PRINT_STR("KB \n");
        
        
        //enable the interrupts
        CPU::enable_intr();


       
    }

    ~KernelEntry() {
        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);

        VGA::PRINT_STR("\nKernel Exitting...");
    }
   

    void Test() {
        VGA::SET_COLOR(VGA::COLORS::MAGENTA, VGA::COLORS::BLACK);
        VGA::PRINT_STR("TESTING VGA :: PF\eAA\eSI\eSL\eEE\eDD\e!");
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
            else panic("Vector Lib Failed!");
        }

        G_OK;

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
