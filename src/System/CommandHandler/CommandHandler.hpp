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

#pragma once

#include "../../lib/core/core.h"
#include "../../System/tty/tty.hpp"
#include "../../System/kout/kout.hpp"
#include <System/Console/console.hpp>
#include "../../lib/StanderdOperations/Operations.hpp"

namespace System
{
    namespace VirtualConsole
    {
        
        struct ParsedCommand
        {
            char *  Command     = "";
            int     CommandLen  = 0;
            char ** Args        = {};
            int     NumArgs     = 0;
        };
        

        // This will handle commands
        class KernelShell : public console {
            public:

            KernelShell(System::Display::tty *tty, i16 ColorF, i16 ColorB);
            ~KernelShell() {};

            void HandleCommand(const char* str);

            private:

            ParsedCommand ParseCommand(char* str);

        };

    } // namespace Shell
    
} // namespace System
