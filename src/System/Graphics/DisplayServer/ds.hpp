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
#include <lib/vector/KernelVector.hpp>
#include <System/Graphics/vbe.hpp>

using namespace System::Graphics;

namespace System
{
    namespace Graphics 
    {
        class lwin
        {
        public:
            lwin();
            ~lwin();

            void init();
            void update();
            void draw(u8* framebuffer);
            void destroy();

            void set_framebuffer(u8* buffer);

            void set_window_title(const char* title);
            void set_window_size(int width, int height);
            void set_window_position(int x, int y);
            void set_window_fullscreen(bool fullscreen);
            void set_window_resizable(bool resizable);
            void set_window_borderless(bool borderless);
            void set_window_maximized(bool maximized);
            void set_window_minimized(bool minimized);
            void set_window_visible(bool visible);
            void set_window_cursor_visible(bool visible);
            void set_window_cursor_grabbed(bool grabbed);
            void set_window_cursor_position(int x, int y);
            void set_window_cursor_type(int type);

            u8* construct_pointer();
            u8* get_framebuffer();

            int get_window_width();
            int get_window_height();
            int get_window_x();
            int get_window_y();
            bool get_window_fullscreen();
            bool get_window_resizable();
            bool get_window_borderless();
            bool get_window_maximized();
            bool get_window_minimized();
            bool get_window_visible();
            bool get_window_cursor_visible();
            bool get_window_cursor_grabbed();
            int get_window_cursor_x();
            int get_window_cursor_y();
            int get_window_cursor_type();
   

            private:
            int width;
            int height;
            int x;
            int y;
            bool fullscreen;
            bool resizable;
            bool borderless;
            bool maximized;
            bool minimized;
            bool visible;
            bool cursor_visible;
            bool cursor_grabbed;
            int cursor_type;
            int cursor_x;
            int cursor_y;
            char* title;

            u8* framebuffer;
        };

        class lux {
            public:

            lux();
            ~lux();

            void init();
            void update();
            void draw();
            void destroy();

            void attach_window(lwin* window);

            private:
            K_Vector<lwin*> windows;
            Driver::GraphicsInfo hardwareInfo;
            u8* doubleframebuffer;
        };

        

    }
}