#include "VGA.h"


namespace STORE {
    int __MODE_SELC = VGA::MODES::TEXT;
    
    int __MODS[6] = {
        80, // size X
        25, // size Y

        VGA::COLORS::BLACK, // Backround Color
        VGA::COLORS::WHITE, // Forground Color

        0, // cusor starting pos X
        0  // cusor starting pos Y
    };

}

namespace BUFFERS {
    struct VGA_BUF
    {
        VGA_VAR     Buff = VGA_PLACEMENT VGA_ADDRESS;
        uint16      size = 0;
        uint8       line_number = 0;
    };

    VGA_BUF DEFAULT_BUFFER = {};

}
    


void VGA::INIT_DISPLAY() {
    BUFFERS::DEFAULT_BUFFER.Buff = VGA_PLACEMENT VGA_ADDRESS;

    VGA::CLEAR_DISPLAY();

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
            if (BUFFERS::DEFAULT_BUFFER.line_number > 24) {
                VGA::CLEAR_DISPLAY();
                return;
            }
            if(BUFFERS::DEFAULT_BUFFER.line_number >= 55){
                BUFFERS::DEFAULT_BUFFER.line_number = 0;
                VGA::RESET();
            }
            BUFFERS::DEFAULT_BUFFER.size = 80 * BUFFERS::DEFAULT_BUFFER.line_number;
            BUFFERS::DEFAULT_BUFFER.line_number++;
        break;
    case '\r':
            BUFFERS::DEFAULT_BUFFER.line_number = 0;
        break;
    case '\t':
            for (int i = 0; i < 5; i ++) {
                VGA::PRINT_CHAR(' ');
            }
        break;
    default:
        BUFFERS::DEFAULT_BUFFER.Buff[BUFFERS::DEFAULT_BUFFER.size] = VGA::VGA_ENTRY(ch, STORE::__MODS[3], STORE::__MODS[2]);
        BUFFERS::DEFAULT_BUFFER.size++;
        break;
    }

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


void INT_TO_A(uint8_t *INPUT_BUFFER, uint32_t base, uint32_t d) {
   uint8_t *p = INPUT_BUFFER;
   uint8_t *p1, *p2;
   uint32_t ud = d;
   uint32_t divisor = 10;

   if(base == 'd' && d < 0) {
       *p++ = '-';
       INPUT_BUFFER++;
       ud = -d;
   } else
     if (base == 'x')
         divisor = 16;

   do{
       uint32_t remainder = ud % divisor;
       *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
   } while (ud /= divisor);

   *p = 0;
   p1 = INPUT_BUFFER;
   p2 = p - 1;
   while (p1 < p2) {
     uint8_t tmp = *p1;
     *p1 = *p2;
     *p2 = tmp;
     p1++;
     p2--;
   }
}


void VGA::kprintf(const char* format, ...) {
    uint8_t **ARGS = (uint8_t **) &format;
    uint8_t CHA;
    uint8_t INPUT_BUFFER[20];

    ARGS++;

    while((CHA = *format++) != 0) {

        if (CHA != '%')
            VGA::PRINT_CHAR (CHA);

        else {
            uint8_t *p, *p2;
            int pad0 = 0, pad = 0;

            CHA = *format++;

            if (CHA == '0') {
                pad0 = 1;
                CHA = *format++;
            }

            if (CHA >= '0' && CHA <= '9') {
                pad = CHA - '0';
                CHA = *format++;
            }

            switch (CHA) {
            case 'X':
            case 'd':
            case 'u':
            case 'x':
                INT_TO_A(INPUT_BUFFER, CHA, *((int *) ARGS++));
                p = INPUT_BUFFER;
                goto string;

                break;

            case 's':
                p = *ARGS++;

                if (! p)
                    p = (uint8_t*)"(null)";

                string:

                for (p2 = p; *p2; p2++) {};
                for (; p2 < p + pad; p2++) {
                    VGA::PRINT_CHAR(pad0 ? '0' : ' ');
                }

                while (*p) {
                    VGA::PRINT_CHAR(*p++);
                }

                break;

            default:
                VGA::PRINT_CHAR(*((int *) ARGS++));
                break;
            }
        }
    }
}