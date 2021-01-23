#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "../core/core.h"
#include "../DisplayOut/displayout.h"
#include "../String/String.h"
#include "../Vector/vector.h"

class console {
    public:

    console();
    ~console();

    // clears the screen
    void cls();

    // Prints to the screen
    void print(const char* str);

    // Goes to the next line
    void nextline();

    // Goes up a line
    void prevline();

    // allows you to go to a line and rewrite over something else (Does not reset line)
    void rewrite(int line, int col, String str);

    // Goes to a line and resets it with the string given
    void drawover(int line, String str);

    void parse_command(const char* command);


    void print_tal();

    private:

    //Vector<String> Data;

        

};



#endif // !CONSOLE_H_
