#ifndef VIRT_CONOSL_H
#define VIRT_CONOSL_H

#include "../core/core.h"

namespace ConsoleInternalCommands
{
    enum Commands {
        TEST = 0,
        HELLO,

        PRINT,
        INFO,
        NOTES,

    };

} // namespace ConsoleInternalCommands


class VirtualConsole {
public:

    VirtualConsole();
    ~VirtualConsole();

    void Handle();

    void updateKeyboard();


    private:

    void parse(char * inp);

    bool ShouldClose = false;

    char key = 0;
    int keybufIndex = 0;

    char  keybuf[80 * 24] = {};


};



#endif // !VIRT_CONOSL_H