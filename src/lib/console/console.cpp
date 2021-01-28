#include "console.h"
#include "../compare/compare.h"


console::console() {
    //print_new_line();
    //print("FluxedOS Console ========");
    //print_new_line();
}

console::~console() {
    
}

// clears the screen
void console::cls() {
    clear_screen();
    this->print_tal();
}

// Prints to the screen
void console::print(const char* str) {
    out(D_INFO, KERNEL, str);
}

// Goes to the next line
void console::nextline() {

}

// Goes up a line
void console::prevline() {

}

// allows you to go to a line and rewrite over something else (Does not reset line)
void console::rewrite(int line, int col, String str) {

}

// Goes to a line and resets it with the string given
void console::drawover(int line, String str) {

}


void console::parse_command(const char* command) {
    char* var[6][4] = {{"test", "tt", "", ""}, {"cls", "clear", "flush", "new"}, {"echo", "printf", "disp", "display"}, {"info", "stat", "dispinfo", ""}, {"help", "", "", ""}, {"halt", "shutdown", "kill", "stop"}};

    enum dipInf {
      dipInfTEST = 1,
      dipInfCLS,
      dipInfECHO,
      dipInfINFO,  
      dipInfHELP,
      dipInfHALT,
    };

    int pos = 0;

    char * spliced_command = "";
    int splcommand = 0;

    for (int i = 0; i < 6; i ++) {
        for (int e = 0; e < 4; e++) {
            
            if (strcmp(command, (const char*)var[i][e]) == 0) {
                pos = i + 1;
                //print_string((char*)command);
                //print_string(" was ");
                //print_string(var[i][e]);
                //print_new_line();

                goto EN;
                // take away the rest of the string
                //for (int f = 0; f < strlen(var[i][e]); f++) {
                //    spliced_command[splcommand] = var[i][e][f];
                //    splcommand++;
                //}
            }

            else {
                //print_string(var[i][e]);
                //print_string(" = ");
                //print_string((char*)command);
                //print_new_line();
            }
        }
    }

    EN:


    switch (pos)
    {
    case dipInfTEST:
        print_string("TEST", RED, GREEN);
        break;

    case dipInfCLS:
        this->cls();
        break;

    case dipInfECHO:
        print_string(spliced_command, GREEN);
        break;

    case dipInfINFO:
        print_string("An Hobbiest OS project", GREEN);
        print_new_line();
        print_string("A OS made by Quest (aka corigan01)!", GREEN);
        break;

    case dipInfHELP:
        print_string("{test, tt} --------------- displays test text", GREEN);
        print_new_line();
        print_string("{cls, clear, flush, new} --------------- clears the screen", GREEN);
        print_new_line();
        print_string("{echo, printf, disp, display} --------------- displays", GREEN);
        print_new_line();
        print_string("{info, stat, dispinfo} --------------- displays info about the OS", GREEN);
        print_new_line();
        print_string("{halt, shutdown, stop, kill} --------------- will stop the os", GREEN);
        print_new_line();
        print_string("{help} --------------- displays this", GREEN);
        break;

    case dipInfHALT:
        closed = true;
        break;

    default:
        out(D_ERROR, KERNEL, "Console could not find a command of that name!");
        break;
    }

    print_new_line();


}


void console::print_tal()  {
        print_string(": \0");
}

bool console::shouldReturn() {
    return this->closed;
}