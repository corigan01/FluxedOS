#include "console.h"


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
}

// Prints to the screen
void console::print(String str) {
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


void console::parse_command(String command) {
    if (command == "CLS") {
        cls();
    }
    else {
        out(D_ERROR, KERNEL, "Console could not find that command!");
    }


}