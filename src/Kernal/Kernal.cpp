#include "Kernal.h"


extern "C"
int kernal_entry() {
	//first init vga with fore & back colors
    init_vga(WHITE, BLACK);

    print_string("FLUXED OS -------");
    print_new_line();

    String str;

    str = "test";
    str = str + str;

    print_string((char*)str.c_str());

    while(1){};
    return 0;
}

