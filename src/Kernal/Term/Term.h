#pragma once

#ifndef Term_H_
#define Term_H_

#include "../core/core.h"


#define VGA_ADDRESS 0xB8000
#define BUFSIZE 8000

static uint16* vga_buffer;


enum vga_color {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GREY,
    DARK_GREY,
    BRIGHT_BLUE,
    BRIGHT_GREEN,
    BRIGHT_CYAN,
    BRIGHT_RED,
    BRIGHT_MAGENTA,
    YELLOW,
    WHITE,
};



//fore & back color values
static uint8 g_fore_color = WHITE, g_back_color = BLACK;
//digit ascii code for printing integers
static int digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

/*
16 bit video buffer elements(register ax)
8 bits(ah) higher : 
  lower 4 bits - forec olor
  higher 4 bits - back color

8 bits(al) lower :
  8 bits : ASCII character to print
*/
uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color);

//clear video buffer array
void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color);
//initialize vga buffer
void init_vga(uint8 fore_color, uint8 back_color);
/*
increase vga_index by width of row(80)
*/
void print_new_line();

//assign ascii character to video buffer
void print_char(char ch, int Fcolor = g_fore_color, int Bcolor = g_back_color);


uint32 strlen(const char* str);

uint32 digit_count(int num);

void itoa(int num, char *number);

//print string by calling print_char
void print_string(char *str, int Fcolor = g_fore_color, int Bcolor = g_back_color);


//print int by converting it into string
//& then printing string
void print_int(int num);


void print_hold_int(int num);

void clear_screen();

#endif