#include "VirtualConsole.h"
#include "../VGA/VGA.h"
#include "../IO/Keyboard.h"
#include "../compare/compare.h"
#include "../mem/mem.h"
#include "../String/String.h"
#include "../Term/Term.h"

VirtualConsole::VirtualConsole() {
    VGA::PRINT_STR("\nStarted VirtualConsole\n");
}

VirtualConsole::~VirtualConsole() {
    VGA::PRINT_STR("\nExiting VirtualConsole\n");
}

void VirtualConsole::Handle() {
    VGA::PRINT_CHAR('>');
    while (!ShouldClose) {
        if (key != NULL) {
            if (key == KEY_BACKSPACE ) {
                if (keybufIndex > 0) {
                    VGA::PRINT_CHAR('\e');
                    keybufIndex--;
                    keybuf[keybufIndex] = NULL;
                }
            }
            else if (key == KEYCODE_ENTER) {
                VGA::PRINT_CHAR('\n');
                

                if (keybufIndex > 0)
                    this->parse(keybuf);

                VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
                VGA::PRINT_CHAR('\n');

                VGA::PRINT_CHAR('>');

                for (int i = 0; i < (80 * 24); i++) {
                    keybuf[i] = NULL;
                }
                keybufIndex = 0;
                key = ' ';
            }
            else  {
                keybuf[keybufIndex] = key;
                keybufIndex++;

                VGA::PRINT_CHAR(key);
            }
            
        
        }
    
        key = getKeydown();
    }

    return;
}

void VirtualConsole::parse(char * inp) {
    unsigned int len = strlen(inp);
    inp[len] = ' ';
    len++;
    
    if (len == 0 || len < 0) {
        return;
    }

    unsigned int start = 0;
    unsigned int end = len;

    if (inp[0] == ' ') {
        for (int i = 0; i < len; i++) {
            if (inp[i] == ' ') {
                start++;
            }
            else {
                break;
            }
        }
    }

    for (int i = start; i < len; i++) {
        if (inp[i] == ' ') {
            end = i;
            break;
        }
    }

    unsigned int commandLen = 0;
    char command[255] = "";

    char args[16][255] = {};

    for (int i = start; i < end; i++) {
        command[commandLen++] = inp[i];
    }

    unsigned int argStart = end;
    unsigned int argEnd = end;
    unsigned int argNum = 0;
    unsigned int argCharNum = 0;


    for (int argP = 0 ; argP < 16 ; argP ++) {
        for (int i = argEnd; i < len; i ++) {
            if (inp[i] == ' ') {
                argStart++;
            }
            else {
                break;
            }
        }

        for (int i = argStart; i < len; i++) {
            if (inp[i] == ' ') {
                argEnd = i;

                for (int e = argStart; e < argEnd; e++) {
                    args[argNum][argCharNum++] = inp[e];

                }

                

                /*VGA::PRINT_STR("Args --> \'");
                VGA::PRINT_STR(args[argNum]);
                VGA::PRINT_STR("\' (Start/End) = {");
                VGA::PRINT_INT(argStart);
                VGA::PRINT_STR(" , ");
                VGA::PRINT_INT(argEnd);
                VGA::PRINT_STR("} \n");
                */

                argCharNum = 0;
                argNum++;
                argStart = argEnd;

                break;
            }
        }
    }
    
    
    
    
    


    

    if (strcmp(command, "exit") == 0) {
        ShouldClose = true;
    }
    else if (strcmp(command, "test") == 0) {
        VGA::SET_COLOR(VGA::COLORS::CYAN, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('T');

        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('E');

        VGA::SET_COLOR(VGA::COLORS::YELLOW, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('S');

        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('T');
    }
    else if (strcmp(command, "memset") == 0) {
        if (argNum == 2) {
            char *inpLoc = args[0];
            char *inpDat = args[1];

            char *value = ((char*)stoi(inpLoc));
            memcpy(value, inpDat, sizeof(char));



            VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
            VGA::PRINT_STR("Done!");
            
        }
        else {
            VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
            VGA::PRINT_STR("You must enter two values (adr*, vlu)!");
        }
    }
    else if (strcmp(command, "memread") == 0) {
        if (argNum == 1) {
            char *inpLoc = args[0];

            char *value = ((char*)stoi(inpLoc));

            VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
            VGA::PRINT_STR("Data: ");
            VGA::PRINT_STR(value);

            VGA::PRINT_STR("\nDone!");

        }
        else {
            VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
            VGA::PRINT_STR("You must enter one value (adr*)!");
        }
    }
    else if (strcmp(command, "echo") == 0) {
        if (argNum > 0) {
            for (int i = 0; i < argNum; i++) {
                VGA::PRINT_STR(args[i]);
                VGA::PRINT_STR(" ");
            }
        }
        else {
            VGA::PRINT_STR("\n");
        }
    }
    else if (strcmp(command, "help") == 0) {
        VGA::PRINT_STR(R"(
Help ----------
Internal Commands :

-    MEMORY   
'memset'  (adr*, vlu) 
'memread' (adr*)     
-    DISPLAY  
'echo' (...) 
-    SYSTEM   
'exit' (void)
-    DEBUG    
'test' (void)
)");
    }
    else {
        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
        VGA::PRINT_STR("ERROR Could not find \'"); 
        VGA::PRINT_STR(command);
        VGA::PRINT_STR("\'!\nDrive and Internal returned \'NOT FOUND\'!");
    }

}