/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#include "CommandHandler.hpp"
#include <System/Display/Display.hpp>

using namespace System;
using namespace System::VirtualConsole;
using namespace System::Display;

BuiltinShell::BuiltinShell(System::Display::tty *tty, i16 ColorF, i16 ColorB) {
    tty->print_str("\nWelcome to BuiltinShell!\nThe FluxedOS Debug Shell\n-------------------------\n");
    this->init(tty, ColorF, ColorB);
}

int BuiltinShell::HandleCommand(const char* str) {
    kout << "R" << endl;
    ParsedCommand Command = this->ParseCommand((char*)str);
   

    #define InternalCommandLen 1
    static const char * InternalCommands[InternalCommandLen] = {
        "exit"
    };

    i8 UsedCommand = 0;
    Command.Command[Command.CommandLen] = '\0';
    Command.CommandLen++;

    for (int i = 0; i < InternalCommandLen; i++) {
        if (strcmp(InternalCommands[i], Command.Command) == 0) {
            UsedCommand = i + 1;
            kout << "User Entered Valid \"" << InternalCommands[i] << "\" Command" << endl;
        }
    }

    
    switch (UsedCommand)
    {
    case 1:
        KillTerm();
        break;
    
    default:
        m_tty->setcolor(Display::Driver::COLOR::RED, Display::Driver::COLOR::BLACK);
        m_tty->print_str("Command \"");
        m_tty->print_str(Command.Command);
        m_tty->print_str("\" Not Found!\n\0");
        break;
    }


    this->ReturnUser();
    kout << "W" << endl;
    return 0;
}

ParsedCommand BuiltinShell::ParseCommand(char* str) {
    i32 len = strlen(str);
    str[len] = ' ';
    len++;
    
    if (len == 0 || len < 0) {
        return {};
    }

    i32 start = 0;
    i32 end = len;

    if (str[0] == ' ') {
        for (int i = 0; i < len; i++) {
            if (str[i] == ' ') {
                start++;
            }
            else {
                break;
            }
        }
    }

    for (int i = start; i < len; i++) {
        if (str[i] == ' ') {
            end = i;
            break;
        }
    }

    i32 commandLen = 0;
    char *command = "";
    char **args = {};


    for (int i = start; i < end; i++) {
        command[commandLen++] = str[i];
    }

    i32 argStart = end;
    i32 argEnd = end;
    i32 argNum = 0;
    i32 argCharNum = 0;


    for (int argP = 0 ; argP < 16 ; argP ++) {
        for (int i = argEnd; i < len; i ++) {
            if (str[i] == ' ') {
                argStart++;
            }
            else {
                break;
            }
        }

        for (int i = argStart; i < len; i++) {
            if (str[i] == ' ') {
                argEnd = i;

                for (int e = argStart; e < argEnd; e++) {
                    args[argNum][argCharNum++] = str[e];

                }

                argCharNum = 0;
                argNum++;
                argStart = argEnd;

                break;
            }
        }
    }

    

    return {command, (int)commandLen, args, (int)argNum};
}