#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"
#include "../lib/Vector/vector.h"
#include "../lib/console/console.h"



void wait(int how_much) {

    print_new_line();
    print_string("System stopped for: s", YELLOW);
    int i = 0;
    while((i / 55000) < how_much){
        i++;

        asm volatile("nop");
        
        
        print_hold_int(i / 55000);
    };
}

int KernStart() {
    init_vga(WHITE, BLACK);


    
    

    char* t = "test";
    if (t == "test") {
        print_string("FLUXED OS ------- build ", GREEN);
        print_int(BUILD);
        print_char('!', GREEN);
        print_new_line();
    }
    wait(2);

    


    
    
    //String str;
    //str = "dfsdf";
    //str.c_str();

    console con;
    

    char *keybuf;
    int keybufIndex = 0;

    for (int i = 0; i < (80 * 24); i++) {
        keybuf[i] = NULL;
    }

    char key = getKeydown();

    print_char('>');

   
    

    while(!con.shouldReturn()) {
        
        
        if (key != NULL) {
            if (key == KEY_BACKSPACE ) {
                if (keybufIndex > 0) {
                    print_char(' ', WHITE, BLACK, 1);
                    keybufIndex--;
                    keybuf[keybufIndex] = NULL;
                }
            }
            else if (key == KEYCODE_ENTER) {
                print_new_line();
                if (keybufIndex > 0)
                    con.parse_command(keybuf);
                print_char('>');

                for (int i = 0; i < (80 * 24); i++) {
                    keybuf[i] = NULL;
                }
                keybufIndex = 0;
                key = ' ';
            }
            else if (keybufIndex < (80*24) - 2) {
                keybuf[keybufIndex] = key;
                keybufIndex++;

                print_char(key);
            }
            else {
                for (int i = 0; i < (80 * 24); i++) {
                    keybuf[i] = NULL;
                }
                keybufIndex = 0;

                clear_screen();
            }
           
        }
        
        key = getKeydown();
        
    }
    
    


    print_new_line();
    print_string("======= OS =======", GREEN);



    return 0;
}
