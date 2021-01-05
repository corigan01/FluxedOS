#pragma once

#ifndef Term_H_
#define Term_H_

#include "../core/core.h"


#define VGA_ADDRESS 0xB8000
#define BUFSIZE 2200

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

//index for video buffer array
static uint32 vga_index;
//counter to store new lines
static uint32 next_line_index = 1;
//fore & back color values
static uint8 g_fore_color = WHITE, g_back_color = BLUE;
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
static uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

//clear video buffer array
static void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  uint32 i;
  for(i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
  }
  next_line_index = 1;
  vga_index = 0;
}

//initialize vga buffer
static void init_vga(uint8 fore_color, uint8 back_color)
{
  vga_buffer = (uint16*)VGA_ADDRESS;
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
}

/*
increase vga_index by width of row(80)
*/
static void print_new_line()
{
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;
}

//assign ascii character to video buffer
static void print_char(char ch, int Fcolor = g_fore_color, int Bcolor = g_back_color, bool hold_ind = false)
{
  vga_buffer[vga_index] = vga_entry(ch, Fcolor, Bcolor);
  if (!hold_ind)
    vga_index++;
}


static uint32 strlen(const char* str)
{
  uint32 length = 0;
  while(str[length])
    length++;
  return length;
}

static uint32 digit_count(int num)
{
  uint32 count = 0;
  if(num == 0)
    return 1;
  while(num > 0){
    count++;
    num = num/10;
  }
  return count;
}

static void itoa(int num, char *number)
{
  int dgcount = digit_count(num);
  int index = dgcount - 1;
  char x;
  if(num == 0 && dgcount == 1){
    number[0] = '0';
    number[1] = '\0';
  }else{
    while(num != 0){
      x = num % 10;
      number[index] = x + '0';
      index--;
      num = num / 10;
    }
    number[dgcount] = '\0';
  }
}

//print string by calling print_char
static void print_string(char *str, int Fcolor = g_fore_color, int Bcolor = g_back_color, bool hold_ind = false)
{
  uint32 index = 0;
  while(str[index]){
    print_char(str[index], Fcolor, Bcolor, hold_ind);
    index++;
  }
}

//print int by converting it into string
//& then printing string
static void print_int(int num, bool hold_ind = false)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num, g_fore_color, g_back_color, hold_ind);
}


#endif