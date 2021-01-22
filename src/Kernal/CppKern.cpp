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



    console con;


    
    
    char* str = "";

    

    int str_index = 0;

    con.print_tal();

    uint8* buf;
    int buf_size = 0;

    bool keyup = true;
    char key_hold = NULL;

    while(1) {
        
        
        char key = get_ascii_char(get_input_keycode());
        print_char(key);
        if (key == NULL) {
            keyup = true;
        }
        else if (key_hold != key && keyup) {
            print_char('=');
            print_char(key);
            key_hold = key;
            keyup = false;
        }
        
        wait(2);
    }
    
    


    print_new_line();
    print_string("======= OS =======", GREEN);


   

    return 0;
}