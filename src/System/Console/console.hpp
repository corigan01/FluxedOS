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

#include <lib/core/core.h>
#include <System/kout/kout.hpp>
#include <System/tty/tty.hpp>


namespace System
{
    namespace VirtualConsole
    {

        class console  {
            public:

            
            //console(System::Display::tty * tty, i16 ColorF, i16 ColorB);
            ~console();

            void begin();

            void HandleKeyCode(i8 keycode);
            void ReturnUser();
            
            void KillTerm();

            char* GetRawCommand();
            bool IsAlive();

            protected:

            void init(System::Display::tty * tty, i16 ColorF, i16 ColorB);
            virtual int HandleCommand(const char* str);


            protected:

            bool HasFinalUserString = false;
            bool HandlerInstalled = false;
            bool TermActive = true;

            i16 ColorF = 1, ColorB = 0;
            i16 CommandLen = 0;
            i8 LastChar = 0;

            char * UserString;
            char * DuUserString;

            System::Display::tty * m_tty;
        };


    } // namespace VirtualConsole
    
} // namespace System
