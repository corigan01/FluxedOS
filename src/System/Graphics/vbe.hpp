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
#include <System/Display/Display.hpp>
#include <System/tty/tty.hpp>
#include <lib/math/math.hpp>
#include <System/Graphics/built-in-font.h>
#include <lib/bitmap/bitmap.hpp>

namespace System 
{

    namespace Graphics 
    {

        namespace Driver 
        {
            

            void gxinit(void* buffer, u32 screen_x, u32 screen_y);
            void putpixel(u32 x,u32 y, u32 color) ;
            void fillrect(u32 color, u32 start_x, u32 start_y, u32 width, u32 hight);
            void linecircle(u32 color, u32 center_x, u32 center_y, u32 rad);
            void fillcircle(u32 color, u32 center_x, u32 center_y, u32 rad);
            void drawchar(u32 ch, u32 x, u32 y, u32 color);
            void drawstring(const char* str, u32 x, u32 y, u32 color);


        }
    }
}