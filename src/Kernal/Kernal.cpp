#include "Kernal.h"

extern "C"
int kernal_entry() {
	//first init vga with fore & back colors
    init_vga(WHITE, BLACK);

    //assign each ASCII character to video buffer
    //you can change colors here
    vga_buffer[0] = vga_entry('H', WHITE, BLACK);
    vga_buffer[1] = vga_entry('e', WHITE, BLACK);
    vga_buffer[2] = vga_entry('l', WHITE, BLACK);
    vga_buffer[3] = vga_entry('l', WHITE, BLACK);
    vga_buffer[4] = vga_entry('o', WHITE, BLACK);
    vga_buffer[5] = vga_entry(' ', WHITE, BLACK);
    vga_buffer[6] = vga_entry('W', WHITE, BLACK);
    vga_buffer[7] = vga_entry('o', WHITE, BLACK);
    vga_buffer[8] = vga_entry('r', WHITE, BLACK);
    vga_buffer[9] = vga_entry('l', WHITE, BLACK);
    vga_buffer[10] = vga_entry('d', WHITE, BLACK);

    while(1){};
    return 0;
}

