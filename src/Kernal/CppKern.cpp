#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"
#include "Vector/vector.h"
#include "console/console.h"

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

    print_string("test");
    print_new_line();

    console con;

    con.print("test");
    con.print_tal();
    
    while(1) {
        char key = get_input_keycode();
        String str;

        switch (key)
        {
        case KEY_ENTER:
            print_new_line();
            con.parse_command(str);


            str = " ";
            con.print_tal();
            break;
        
        default:
            str = str + (char*)get_ascii_char(key);
            print_char(get_ascii_char(key), BLUE, BLACK);
            break;
        }

        wait(2);
    }
    con.print_tal();
    
    

    print_string("test2");
    print_new_line();

    print_new_line();
    print_string("======= OS =======", GREEN);


   

    return 0;
}