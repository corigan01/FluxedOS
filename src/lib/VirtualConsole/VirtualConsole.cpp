#include "VirtualConsole.h"
#include "../VGA/VGA.h"
#include "../IO/Keyboard.h"
#include "../compare/compare.h"
#include "../mem/mem.h"
#include "../String/String.h"
#include "../hal/hal.h"



VirtualConsole::VirtualConsole() {
    VGA::PRINT_STR("\nStarted VirtualConsole\n");
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
                

                if (keybufIndex > 0) {
                    int e = tee(parse(keybuf));


                }

                VGA::PRINT_CHAR('\n');
                VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
                

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

    
}

int VirtualConsole::tee(VirtualConsoleParsed arg) {
    static const char *internalCommands[16] = {
        "exit",
        "fluxed",
        "test",
        "memread",
        "memset",
        "help",
        "panic",
        "reset",
        "shutdown",
        "throw"

    };

    arg.Command[arg.CommandLen] = '\0';

    int numSelected = -1;
    for (int i = 0; i < 16; i++) {
        if (strcmp(internalCommands[i], arg.Command) == 0) {
            numSelected = i;
            break;
        }
    }

    switch (numSelected)
    {
    case 0:
        ShouldClose = 1;
        break;

    case 1:
        VGA::PRINT_STR(R"(    ________                    ______  _____
   / ____/ /_  ___  _____  ____/ / __ \/ ___/
  / /_  / / / / / |/_/ _ \/ __  / / / /\__ \ 
 / __/ / / /_/ />  </  __/ /_/ / /_/ /___/ / 
/_/   /_/\__,_/_/|_|\___/\__,_/\____//____/  
A hobby OS Project! Made by Main Menu aka corigan01 )");
        break;

    case 2:
        VGA::SET_COLOR(VGA::COLORS::CYAN, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('T');

        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('E');

        VGA::SET_COLOR(VGA::COLORS::YELLOW, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('S');

        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
        VGA::PRINT_CHAR('T');
        break;

    case 3:
    {
        char *inpLoc = arg.Args[0];
        char *value = ((char*)stoi(inpLoc));

        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
        VGA::PRINT_STR("Data: ");
        VGA::PRINT_STR(value);

        VGA::PRINT_STR("\nDone!");
    }
        break;
    
    case 4:
    {
        char *inpLoc = arg.Args[0];
        char *inpDat = arg.Args[1];

        char *value = ((char*)stoi(inpLoc));
        memcpy(value, inpDat, sizeof(char));



        VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
        VGA::PRINT_STR("Done!");
    }
        break;
    
    case 5:
        break;

    case 6:
        panic("PANIC called from TERM");
        break;

    case 7:
        SYS::reset();
        break;

    case 8:
        SYS::shutdown();
        break;

    case 9:
        ThrowISR(stoi(arg.Args[0]));
        break;

    default:
        VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
        VGA::PRINT_STR("ERROR Could not find \'"); 
        VGA::PRINT_STR(arg.Command);
        VGA::PRINT_STR("\'!\nDrive and Internal returned \'NOT FOUND\'!");
        break;
    }

    return 0;
}

VirtualConsoleParsed VirtualConsole::parse(char * inp) { 
    unsigned int len = strlen(inp);
    inp[len] = ' ';
    len++;
    
    if (len == 0 || len < 0) {
        return {};
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
    char *command = "";
    char **args = {};

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

                argCharNum = 0;
                argNum++;
                argStart = argEnd;

                break;
            }
        }
    }

    
    

    return {command, commandLen, args, argNum};

}