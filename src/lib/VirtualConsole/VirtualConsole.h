#ifndef VIRT_CONOSL_H
#define VIRT_CONOSL_H

#include "../VGA/VGA.h"
#include "../IO/Keyboard.h"



class VirtualConsole {
public:

    VirtualConsole() {
        VGA::PRINT_STR("\nStarted VirtualConsole\n");
    }
    ~VirtualConsole() {
        VGA::PRINT_STR("\nExiting VirtualConsole\n");
    }

    void Handle() {
        
    
    }

    void updateKeyboard() {
        ;
    }


};



#endif // !VIRT_CONOSL_H