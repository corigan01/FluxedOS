#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"
#include "../lib/Vector/vector.h"
#include "../lib/console/console.h"

void wait(int how_much) {

    uint32 timer_count = (uint32)how_much * 0x02FFFFFF;

    while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

int KernStart() {
    init_vga(WHITE, BLACK);

    print_string("FLUXED OS ------- build ", GREEN);
    print_int(BUILD);
    print_char('!', GREEN);
    print_new_line();

    wait(2);

    //String str;
    //str = "dfsdf";
    //str.c_str();

    char keybuf[80*60] = {};
    int keybufIndex = 0;

    char key = getKeydown();
    while(1) {
        
        
        if (key != '_') {
            if (keybufIndex < (80 * 60) - 2) {
                keybuf[keybufIndex] = key;
                keybufIndex++;
            }
            else {
                for (int i = 0; i < (80 * 60); i++) {
                    keybuf[i] = NULL;
                }
                keybufIndex = 0;

                keybuf[keybufIndex] = key;
                keybufIndex++;
                
                clear_screen();
            }
            print_char(key);
        }
        
        key = getKeydown();
        
    }
    
    


    print_new_line();
    print_string("======= OS =======", GREEN);


   

    return 0;
}
