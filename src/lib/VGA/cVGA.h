#ifndef CVGA__VGA__H
#define CVGA__VGA__H




#include "../core/core.h"



EXTNC_ void CLEAR_DISPLAY();

EXTNC_ void PRINT_CHAR(char c);
EXTNC_ void PRINT_STR(const char * s);
EXTNC_ void SET_COLOR(uint8 a, uint8 b);
EXTNC_ void PRINT_INT(int i);



// colors
enum __COLORS {
    __BLACK = 0,
    __BLUE,
    __GREEN,
    __CYAN,
    __RED,
    __MAGENTA,
    __BROWN,
    __GREY,
    __DARK_GREY,
    __BRIGHT_BLUE,
    __BRIGHT_GREEN,
    __BRIGHT_CYAN,
    __BRIGHT_RED,
    __BRIGHT_MAGENTA,
    __YELLOW,
    __WHITE,
};


#endif // !CVGA__VGA__H
