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



    console con;


    
    
    char* str = "";

    

    int str_index = 0;

    con.print_tal();

    while(1) {
        char key = get_input_keycode();
        

        switch (key)
        {
        case KEY_ENTER:
            print_new_line();
            con.parse_command(str);
            

            str = "";
            str_index = 0;
            con.print_tal();

            
            break;
        
        default:
            print_char(get_ascii_char(key), MAGENTA, BLACK);
            
            str[str_index] = get_ascii_char(key);
            str_index++;

            //print_string((char*)str);
            break;
        }

        if (get_line_index() > 25) {
            print_int(get_vga_index());
            con.cls();
        }

        wait(2);
    }
    
    


    print_new_line();
    print_string("======= OS =======", GREEN);


   

    return 0;
}