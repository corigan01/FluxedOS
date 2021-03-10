#pragma once

#ifndef TEXT_MODE_H_
#define TEXT_MODE_H_

#include "../core/core.h"

#define VGA_ADDRESS 0xB8000
#define VGA_VAR uint16*
#define VGA_PLACEMENT (uint16*)

namespace VGA
{
    namespace MODES {
        enum __VGA__MODES {
            TEXT = 0
        };
    }
    namespace MODS {
        enum __VGA__MOD {
            SIZE_X = 0,
            SIZE_Y,

            DEF_COLOR_BACKROUND,
            DEF_COLOR_FORGROUND,

            CURSOR_X,
            CURSOR_Y,
        };
    }
    namespace COLORS {
        enum __VGA__COLORS {
            BLACK = 0,
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
    }

    


    void INIT_DISPLAY();
    void SET_DISPLAY_MODE(MODES::__VGA__MODES mode);
    void SET_MODS(MODS::__VGA__MOD modID, int mod);

    void CLEAR_DISPLAY();
    void RESET();

    uint16 VGA_ENTRY(char ch, uint8 fore_color, uint8 back_color);
    void PRINT_CHAR(char ch);
    void PRINT_STR(const char * str);
    void SET_COLOR(uint8 fore_color, uint8 back_color);
    void kprintf(const char* format, ...);
    void PRINT_INT(int in);






} // namespace VGA

#define panic(x) \
    VGA::CLEAR_DISPLAY(); \
    VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK); \
    VGA::PRINT_STR("==KERNEL PANIC== [ERROR TYPE: UNRECOVERABLE]\n trace: ["); \
    VGA::PRINT_STR(__FILE__); \
    VGA::PRINT_STR(": "); \ 
    VGA::PRINT_INT(__LINE__); \ 
    VGA::PRINT_STR("]\n cause --> \'"); \
    VGA::PRINT_STR(x); \
    VGA::PRINT_STR("\' \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); \
    HALT; 



#endif // !TEXT_MODE_H_
