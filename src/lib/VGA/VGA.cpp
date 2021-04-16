#include "VGA.h"
#include "../mem/mem.h"
#include "../hal/hal.h"
#include "../String/String.h"
#include "cVGA.h"
#include "../IO/Serial/serial.h"
#include "../compare/compare.h"



    


void VGA::INIT_DISPLAY() {
    BUFFERS::DEFAULT_BUFFER.line_number = 25;
    BUFFERS::DEFAULT_BUFFER.size = 0;
    BUFFERS::DEFAULT_BUFFER.Buff = VGA_PLACEMENT VGA_ADDRESS;

    //VGA::CLEAR_DISPLAY();
    VGA::PRINT_CHAR('\n');

}

void VGA::SET_DISPLAY_MODE(MODES::__VGA__MODES mode) {
    
    STORE::__MODE_SELC = mode;

    VGA::RESET();
}

void VGA::SET_MODS(MODS::__VGA__MOD modID, int mod){
    STORE::__MODS[modID] = mod;

    VGA::RESET();
}

void VGA::CLEAR_DISPLAY() {

    VGA::RESET();

    for (int i = 0; i < BUFFERS::DEFAULT_BUFFER.size; i++) {
        for (int e = 0; e < 80; e++) {
            VGA::PRINT_CHAR(' ');
        }
    }

    VGA::RESET();
    VGA::PRINT_CHAR('\n');
}

void VGA::RESET(){
    uint16 BUFFERSIZE = STORE::__MODS[0] * STORE::__MODS[1];
    for(int i = 0; i < BUFFERSIZE; i++){
        BUFFERS::DEFAULT_BUFFER.Buff[i] = VGA::VGA_ENTRY(NULL, STORE::__MODS[3], STORE::__MODS[2]);
    }
    BUFFERS::DEFAULT_BUFFER.line_number = 0;
    BUFFERS::DEFAULT_BUFFER.size = 0;
}

uint16 VGA::VGA_ENTRY(char ch, uint8 fore_color, uint8 back_color) {
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

void VGA::PRINT_CHAR(char ch){
    switch (ch)
    {
    case '\n':
            if (BUFFERS::DEFAULT_BUFFER.line_number > 24 || true) {
                
                uint16 * VBUF;
                int bufS = 80*80;
                memcpy(VBUF, BUFFERS::DEFAULT_BUFFER.Buff, bufS);

                int VGA_INT = 0;
                for (int i = 80 ; i < bufS - 80*2 ; i++) {
                    BUFFERS::DEFAULT_BUFFER.Buff[VGA_INT++] = VBUF[i];
                }

                BUFFERS::DEFAULT_BUFFER.line_number = 24;
                BUFFERS::DEFAULT_BUFFER.size = 80*24;

                for (int i = 0; i < 80; i++) {
                    BUFFERS::DEFAULT_BUFFER.Buff[BUFFERS::DEFAULT_BUFFER.size + i] = VGA::VGA_ENTRY(' ', VGA::COLORS::WHITE, VGA::COLORS::BLACK);
                }

                

                
            }
            else {
                BUFFERS::DEFAULT_BUFFER.size = 80 * BUFFERS::DEFAULT_BUFFER.line_number;
                BUFFERS::DEFAULT_BUFFER.line_number++;
                //incLine(); 
            }

            RawSerialoutc('\n');
            
        break;
    case '\r':
            BUFFERS::DEFAULT_BUFFER.line_number--;
            VGA::PRINT_CHAR('\n');
        break;
    case '\t':
            for (int i = 0; i < 5; i ++) {
                VGA::PRINT_CHAR(' ');
            }
        break;
    case '\e':
        BUFFERS::DEFAULT_BUFFER.size--;
        BUFFERS::DEFAULT_BUFFER.Buff[BUFFERS::DEFAULT_BUFFER.size] = VGA_ENTRY(' ', VGA::COLORS::WHITE, VGA::COLORS::BLACK);
        break;
    default:
        if (BUFFERS::DEFAULT_BUFFER.line_number > 0) {

            // Fix me [Chars can go off screen]
            //if (BUFFERS::DEFAULT_BUFFER.size / (BUFFERS::DEFAULT_BUFFER.line_number ) > 80) {
            //
                //VGA::PRINT_CHAR('\n');
            //}
        }
       
        
        BUFFERS::DEFAULT_BUFFER.Buff[BUFFERS::DEFAULT_BUFFER.size] = VGA::VGA_ENTRY(ch, STORE::__MODS[3], STORE::__MODS[2]);
        BUFFERS::DEFAULT_BUFFER.size++;
        RawSerialoutc(ch);

        break;
    }

    VGA::CURSOR::UPDATE(BUFFERS::DEFAULT_BUFFER.size - (BUFFERS::DEFAULT_BUFFER.line_number * 80), BUFFERS::DEFAULT_BUFFER.line_number + 0);

}

void VGA::PRINT_STR(const char * str){
    uint32 length = 0;
    while(str[length] != '\0')
        length++;

   for (int i = 0; i < length; i++) {
       VGA::PRINT_CHAR(str[i]);
   }
   

}

void VGA::SET_COLOR(uint8 fore_color, uint8 back_color) {
    STORE::__MODS[3] = fore_color;
    STORE::__MODS[2] = back_color;
}


void VGA::PRINT_INT(int in) { 
    char str_num[digit_count(in)+1];
    itoa(in, str_num);
    VGA::PRINT_STR(str_num);
}



void VGA::kprintf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    VGA::fmat(fmt, [](int ch) { VGA::PRINT_CHAR(ch); }, va);
    va_end(va);
    
}

template<typename T>
void VGA::fmat(const char* fmt, T emit, va_list va)
{
    int isLong = 0;
    auto fetchValue = [&]() -> uintmax_t {
        int wasLong = isLong;
        isLong = 0;
        switch (wasLong) {
            case 0:
                return va_arg(va, unsigned int);
            case 1:
                return va_arg(va, unsigned long);
            case 2:
                return va_arg(va, unsigned long long);
        }
        return -1;
    };

    for (; *fmt != '\0'; ++fmt) {
        if (*fmt != '%') {
            emit(*fmt);
            continue;
        }

        fmt++;
        while (*fmt == 'l') {
            ++isLong, ++fmt;
        }

        switch (*fmt) {
            case 'c': {
                char ch = va_arg(va, int);
                emit(ch);
                break;
            }
            case 'p': {
                const uint32 v = va_arg(va, unsigned long);
                VGA::PRINT_INT(v);
                isLong = 0;
                break;
            }
            case 'x': {
                const uintmax_t v = fetchValue();
                VGA::PRINT_INT(v);
                break;
            }
            case 's': {
                const char* s = va_arg(va, const char*);
                while (*s != '\0')
                    emit(*s++);
                break;
            }
            case 'd': {
                const uintmax_t v = fetchValue();
                VGA::PRINT_INT(v);
                break;
            }
            default:
                emit('%');
                emit(*fmt);
                break;
        }
    }
}

// ---------------
// C call layer
// ---------------


EXTNC_ void CLEAR_DISPLAY() {
    VGA::CLEAR_DISPLAY();
}

EXTNC_ void PRINT_CHAR(char c) {
    VGA::PRINT_CHAR(c);
}

EXTNC_ void PRINT_STR(const char * s) {
    VGA::PRINT_STR(s);
}

EXTNC_ void SET_COLOR(uint8 a, uint8 b) {
    VGA::SET_COLOR(a, b);
}

EXTNC_ void PRINT_INT(int i) {
    VGA::PRINT_INT(i);
}

EXTNC_ void INIT_DISPLAY() {
    VGA::INIT_DISPLAY();
}



void VGA::CURSOR::ENABLE(uint8_t cursor_start, uint8_t cursor_end) {
    port_byte_out(0x3D4, 0x0A);
	port_byte_out(0x3D5, (port_byte_in(0x3D5) & 0xC0) | cursor_start);
 
	port_byte_out(0x3D4, 0x0B);
	port_byte_out(0x3D5, (port_byte_in(0x3D5) & 0xE0) | cursor_end);  
}

void VGA::CURSOR::DISABLE() {
    port_byte_out(0x3D4, 0x0A);
	port_byte_out(0x3D5, 0x20);
}

void VGA::CURSOR::UPDATE(int x, int y) {
    uint16_t pos = y * 80 + x;
 
	port_byte_out(0x3D4, 0x0F);
	port_byte_out(0x3D5, (uint8_t) (pos & 0xFF));
	port_byte_out(0x3D4, 0x0E);
	port_byte_out(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16 VGA::CURSOR::GET(){   
    uint16_t pos = 0;
    port_byte_out(0x3D4, 0x0F);
    pos |= port_byte_in(0x3D5);
    port_byte_out(0x3D4, 0x0E);
    pos |= ((uint16_t)port_byte_in(0x3D5)) << 8;
    return pos;
}   