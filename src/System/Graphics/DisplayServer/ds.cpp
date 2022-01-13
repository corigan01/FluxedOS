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

#include <System/Graphics/DisplayServer/ds.hpp>
#include <System/memory/kmemory.hpp>
#include <lib/vector/KernelVector.hpp>

using namespace System::Graphics;



lux::lux() { 

}
lux::~lux() { 
    this->destroy();
}

void lux::init() { 
    kout << "Initializing Lux instance..." << endl;
    hardwareInfo = Driver::getinfo();
    kout << "Hardware info: " << endl;
    kout << "  - Screen width       : " << hardwareInfo.width << endl;
    kout << "  - Screen height      : " << hardwareInfo.height << endl;
    kout << "  - Screen pitch       : " << hardwareInfo.pitch << endl;
    kout << "  - Screen pixelwidth  : " << hardwareInfo.pixelwidth << endl;
    kout << "  - Memory addr        : *" << (u32)hardwareInfo.buffer << endl;

    doubleframebuffer = (u8*)System::Memory::kmalloc(hardwareInfo.width * hardwareInfo.height);
}
void lux::update() { 
    // flip double framebuffer
    memcpy(hardwareInfo.buffer, doubleframebuffer, hardwareInfo.width * hardwareInfo.height * hardwareInfo.pitch);
}
void lux::draw() { 
    for (int i = 0; i < windows.size(); i++) {
        windows[i]->draw(doubleframebuffer);
    }
}
void lux::destroy() { 
    kout << "Destroying Lux instance..." << endl;
    for (int i = 0; i < windows.size(); i++) {
        windows[i]->destroy();
    }
    System::Memory::kfree(doubleframebuffer);
}

void lux::attach_window(lwin* window) {
    this->windows.push_back(window);
}

lwin::lwin() { 

}
lwin::~lwin() { 

}

void lwin::init() { 

}
void lwin::update() { 

}
void lwin::draw(u8* framebuffer) { 
    // do fancy drawing here
    // otherwise just draw the framebuffer

    // draw the framebuffer
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            framebuffer[(y * width) + x] = this->framebuffer[y+x];
        }
    }
}
void lwin::destroy() { 

}

u8* lwin::construct_pointer() { 
    return (u8*)System::Memory::kmalloc(this->width * this->height); 
}

void lwin::set_framebuffer(u8* buffer) {
    this->framebuffer = buffer;
}

void lwin::set_window_title(const char* title) { 
    this->title = (char*)title;
}
void lwin::set_window_size(int width, int height) { 
    this->width = width;
    this->height = height;
}
void lwin::set_window_position(int x, int y) { 
    this->x = x;
    this->y = y;
}
void lwin::set_window_fullscreen(bool fullscreen) { 
    this->fullscreen = fullscreen;
}
void lwin::set_window_resizable(bool resizable) { 
    this->resizable = resizable;
}
void lwin::set_window_borderless(bool borderless) { 
    this->borderless = borderless;
}
void lwin::set_window_maximized(bool maximized) { 
    this->maximized = maximized;
}
void lwin::set_window_minimized(bool minimized) { 
    this->minimized = minimized;
}
void lwin::set_window_visible(bool visible) { 
    this->visible = visible;
}
void lwin::set_window_cursor_visible(bool visible) { 
    this->cursor_visible = visible;
}
void lwin::set_window_cursor_grabbed(bool grabbed) { 
    this->cursor_grabbed = grabbed;
}
void lwin::set_window_cursor_position(int x, int y) { 
    this->cursor_x = x;
    this->cursor_y = y;
}
void lwin::set_window_cursor_type(int type) { 
    this->cursor_type = type;
}
    