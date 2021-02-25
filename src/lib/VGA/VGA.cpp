#include "VGA.h"
#include "../Term/Term.h"
#include "../mem/mem.h"


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
    BUFFERS::DEFAULT_BUFFER.line_number = getLine();
    BUFFERS::DEFAULT_BUFFER.size = getBuf();
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
            if (BUFFERS::DEFAULT_BUFFER.line_number >= 24) {
                uint16 * VBUF;
                memcpy(VBUF, BUFFERS::DEFAULT_BUFFER.Buff, 80*24);

                VGA::CLEAR_DISPLAY();

                int VGA_INT = 0;
                for (int i = 80; i < 80*24 ; i++) {
                    BUFFERS::DEFAULT_BUFFER.Buff[VGA_INT++] = VBUF[i];
                }

                BUFFERS::DEFAULT_BUFFER.line_number = 24;
                BUFFERS::DEFAULT_BUFFER.size = 80 * 24;

                VGA::PRINT_CHAR(':');
            }
            else {
                BUFFERS::DEFAULT_BUFFER.size = 80 * BUFFERS::DEFAULT_BUFFER.line_number;
                BUFFERS::DEFAULT_BUFFER.line_number++;
                incLine(); 
            }
            
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
        BUFFERS::DEFAULT_BUFFER.Buff[BUFFERS::DEFAULT_BUFFER.size] = VGA_ENTRY(' ', WHITE, BLACK);
        break;
    default:
        if (BUFFERS::DEFAULT_BUFFER.line_number > 0) {
            if (BUFFERS::DEFAULT_BUFFER.size / (BUFFERS::DEFAULT_BUFFER.line_number ) > 80) {
                VGA::PRINT_CHAR('\n');
            }
        }
        
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


void VGA::PRINT_INT(int in) { 
    char str_num[digit_count(in)+1];
    itoa(in, str_num);
    VGA::PRINT_STR(str_num);
}

void VGA::kprintf(const char* format, ...) {
    char **ARGS = (char **) &format;
    int InputVarSize = 0;


    char** inputText;
    char *c;
    while (c = *ARGS++) {
        int i = 0; while(c[i++] != 0);
        
        // FIXME
        // InputVarSize is always = 6
        // This was added to fix the problem, and inded it did to an extent
        // This broke all numbers as it set them to ~104399 or something
        // Also new lines are broken as it takes some text and moves it to the other line 
        // 
        // Test Output:
        // This is some te
        // at is working! xt th
        //
        // What it should look like:
        // This is some te
        // xt that is working!
        //
        //if (i > 1) {
            inputText[InputVarSize++] = c;
        //}
    }

    char * FirstText = inputText[0];
    int usedVarIndex = 1;
    
    if (InputVarSize > 0) {
        int FirstTextSize = strlen(FirstText);
        for (int i = 0; i < FirstTextSize; i++) {
            if (FirstText[i] == '%') {
                if (FirstTextSize > i) {
                    
                    if (usedVarIndex >= InputVarSize) {
                        PRINT_STR("%[That VAR was not given!]");
                        
                    }
                    else {
                        switch (FirstText[i + 1])
                        {
                        case ' ':
                        case '\0':
                            PRINT_CHAR('%');
                            i--;
                            break;
                        case 's':
                            PRINT_STR(inputText[usedVarIndex++]);
                            break;

                        case 'd':
                            char* in;
                            itoa((int)inputText[usedVarIndex++], in);
                            PRINT_STR(in);
                            break;
                        
                        default:
                            PRINT_STR("%[Unknown char \'");
                            PRINT_CHAR(FirstText[i + 1]);
                            PRINT_STR("\'!]");
                            usedVarIndex++;
                            break;
                        }
                    }
                    i++;
                }
            }
            else {
                PRINT_CHAR(FirstText[i]);
            }
        }
    }

    PRINT_STR("\n");


}