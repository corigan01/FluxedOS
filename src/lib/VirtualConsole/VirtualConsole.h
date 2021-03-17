#ifndef VIRT_CONOSL_H
#define VIRT_CONOSL_H

#include "../core/core.h"



struct VirtualConsoleParsed
{
    char *  Command     = "";
    int     CommandLen  = 0;
    char ** Args        = {};
    int     NumArgs     = 0;
};

class VirtualConsole {
public:

    VirtualConsole();
    

    void Handle();

    void updateKeyboard();


    private:

    VirtualConsoleParsed parse(char * inp);
    int tee(VirtualConsoleParsed arg);

    bool ShouldClose = false;

    char key = 0;
    int keybufIndex = 0;

    char  keybuf[80 * 24] = {};


};



#endif // !VIRT_CONOSL_H