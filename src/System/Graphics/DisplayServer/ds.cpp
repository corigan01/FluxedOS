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
    doubleframebuffer = (u8*)System::Memory::kmalloc(hardwareInfo.width * hardwareInfo.height * hardwareInfo.pitch);

    kout << "Hardware info: " << endl;
    kout << "  - Screen width       : " << hardwareInfo.width << endl;
    kout << "  - Screen height      : " << hardwareInfo.height << endl;
    kout << "  - Screen pitch       : " << hardwareInfo.pitch << endl;
    kout << "  - Screen pixelwidth  : " << hardwareInfo.pixelwidth << endl;
    kout << "  - Memory addr        : *" << (u32)doubleframebuffer << endl;
}

u8* lux::get_framebuffer() { 
    return doubleframebuffer;
}

void lux::flip_buffer() { 
    // flip double framebuffer
    memcpy(hardwareInfo.buffer, doubleframebuffer, hardwareInfo.width * hardwareInfo.height * hardwareInfo.pitch);
}
void lux::draw_windows() { 
    //memset(doubleframebuffer, 0x00, hardwareInfo.width * hardwareInfo.height * hardwareInfo.pitch);
    for (int i = 0; i < windows.size(); i++) {
        windows[i]->draw(doubleframebuffer, hardwareInfo.width, hardwareInfo.height);
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
    window->attach_windowServer(this);

    this->windows.push_back(window);
}

void lux::destroy_window(lwin* window) {
    kout << "Destoried window : " << kout.ToHex((u32)window) << endl;
    for (int i = 0; i < this->windows.size(); i++) {
        if (this->windows[i] == window) this->windows.pop_at(i);
    }

    this->draw_windows();
    this->flip_buffer();
}

void lux::redraw_one_window(lwin* windowpointer) {
    for (int i = 0; i < this->windows.size(); i++) {
        if (this->windows[i] == windowpointer) windows[i]->draw(hardwareInfo.buffer, hardwareInfo.width, hardwareInfo.height);
    }
}


lwin::lwin() { 

}
lwin::~lwin() { 

    this->WindowServerPointer->destroy_window(this);

    this->destroy();
}

void lwin::init() { 
    
    //memset(this->titlebar, 0x00, 10 * this->width * pitch);

}
void lwin::update() { 

}

void lwin::attach_windowServer(System::Graphics::lux * lux_instance) {
    this->WindowServerPointer = lux_instance;
}

void lwin::draw(u8* framebuffer, u32 ypull, u32 xpull) { 
    // do fancy drawing here
    // otherwise just draw the framebuffer

    // draw the framebuffer
    /*kout << "Drawing Window" << endl;
    kout << "  - Pos X          : " << this->x << endl;
    kout << "  - Pos Y          : " << this->y << endl;
    kout << "  - Window Width   : " << this->width << endl;
    kout << "  - Window Hight   : " << this->height << endl;
    kout << "  - Framebuffer    : *" << (u32)this->framebuffer << "  -->  *" << (u32)framebuffer << endl;*/

    const int titlebarsize = this->fullscreen ? 0 : 15;

    

    int screenwidth = this->width;
    int screenhight = this->height;
    int startingx = this->x;
    int startingy = this->y;

    if (!this->fullscreen) {
        this->fillrect_titlebar(0x555555, 0, 0, width, titlebarsize);
        this->drawstring_titlebar(this->title, 0, 0, 0xFFFFFF);
        for (int e = 0; e < titlebarsize; e++) {
            for (int r = 0; r < this->width * pitch; r ++) {
                framebuffer[(e * (ypull * pitch)) + r + ((startingx * pitch) + (startingy * ypull * pitch))] = this->titlebar[e * pitch * width + r];
            }
        }
    }
    
    
    for (int hi = (titlebarsize * 1); hi < screenhight + (titlebarsize * 1); hi++) {
        for (int i = 0; i < screenwidth * this->pitch; i++) {
            if (hi < this->height * pitch && i < this->width * this->pitch)
                framebuffer[(hi * (ypull * pitch)) + i + ((startingx * pitch) + (startingy * ypull * pitch))] = this->framebuffer[(hi - (titlebarsize * 1)) * pitch * width + i ];      
            //else
                 //framebuffer[(hi * (ypull * pitch)) + i + ((startingx * pitch) + (startingy * ypull * pitch))] = 0x22;
        }
    }
}
void lwin::destroy() { 
    System::Memory::kfree(this->framebuffer);
    System::Memory::kfree(this->titlebar);
}

u8* lwin::construct_pointer() { 
    Driver::GraphicsInfo info = Driver::getinfo();

    this->titlebar = (u8*)System::Memory::kmalloc(15 * info.width * pitch);
    memset(this->titlebar, 0x00, 15 * info.width * pitch);

    u8* pt = (u8*)System::Memory::kmalloc(info.pitch * info.width * info.height); 
    memset(pt, 0x00, info.pitch * info.width * info.height);
    this->framebuffer = pt;
    return pt;
}

void lwin::redraw_window() {
    this->WindowServerPointer->redraw_one_window(this);
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
    this->x_F = this->x;
    this->y_F = this->y;
    this->width_F = this->width;
    this->height_F = this->height;

    this->x = 0;
    this->y = 0;

    this->width = Driver::getinfo().width;
    this->height = Driver::getinfo().height;
    
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
int lwin::get_window_width() {
    return this->width;
}
int lwin::get_window_height() {
    return this->height;
}
int lwin::get_window_x() {
    return this->x;
}
int lwin::get_window_y() {
    return this->y;
}
bool lwin::get_window_fullscreen() {
    return this->fullscreen;
}
bool lwin::get_window_resizable() {
    return this->resizable;
}
bool lwin::get_window_borderless() {
    return this->borderless;
}
bool lwin::get_window_maximized() {
    return this->maximized;
}
bool lwin::get_window_minimized() {
    return this->minimized;
}
bool lwin::get_window_visible() {
    return this->visible;
}
bool lwin::get_window_cursor_visible() {
    return this->cursor_visible;
}
bool lwin::get_window_cursor_grabbed() {
    return this->cursor_grabbed;
}
int lwin::get_window_cursor_x() {
    return this->cursor_x;
}
int lwin::get_window_cursor_y() {
    return this->cursor_y;
}
int lwin::get_window_cursor_type() {
    return this->cursor_type;
}

void lwin::putpixel(int x, int y, u32 color) {
    //if (x < 0 || x >= this->x || y < 0 || y >= this->y) return;
    u32 where = (x * pitch) + (y * pitch * this->width);
    this->framebuffer[where] = color & 255;              // BLUE
    this->framebuffer[where + 1] = (color >> 8) & 255;   // GREEN
    this->framebuffer[where + 2] = (color >> 16) & 255;  // RED
}

void lwin::putpixel_titlebar(int x, int y, u32 color) {
    //if (x < 0 || x >= this->x || y < 0 || y >= this->y) return;
    u32 where = (x * pitch) + (y * pitch * this->width);
    this->titlebar[where] = color & 255;              // BLUE
    this->titlebar[where + 1] = (color >> 8) & 255;   // GREEN
    this->titlebar[where + 2] = (color >> 16) & 255;  // RED
}

void lwin::fillrect(u32 color, u32 start_x, u32 start_y, u32 width, u32 hight) {
    for (u32 y = start_y; y < hight + start_y; y++) {
        for (u32 x = start_x; x < width + start_x; x++) {
            this->putpixel(x, y, color);
        }
    }
}

void lwin::linecircle(u32 color, u32 center_x, u32 center_y, u32 rad) {
    double i = 0, angle = 0, x1 = 0, y1 = 0;

    for(i = 0; i < 360; i += 1) {
        angle = i;
        
        x1 = rad * math::cos(angle * math::PI / 180.00);
        y1 = rad * math::sin(angle * math::PI / 180.00);

        if (center_x + x1 > 0 && center_y + y1 > 0)
            this->putpixel(center_x + x1, center_y + y1, color);
    }   
}

void lwin::fillcircle(u32 color, u32 center_x, u32 center_y, u32 rad) {
    for (int w = 0; w < rad * 2; w++)
    {
        for (int h = 0; h < rad * 2; h++)
        {
            int dx = rad - w; // horizontal offset
            int dy = rad - h; // vertical offset

            if ((dx*dx + dy*dy) <= (rad * rad)) {
                this->putpixel(center_x + dx, center_y + dy, color);
            }
        }
    }
}

void lwin::drawchar(u32 ch, u32 x, u32 y, u32 color) {
    const unsigned char * gliph = built_in_font[ch - 32];
    
    for(int cy = 0; cy < 13; cy++) {
        u8 byte = gliph[cy];

        for (int i = 0; i < 8; i ++) {
            if (CHECK_BIT(byte, i)) {
                this->putpixel((8 - i) + x, 13 - cy + y, color);
                //kout << "-";
            }
        }
    }
}

void lwin::drawstring(const char* str, u32 x, u32 y, u32 color) {
    for (int i = 0; i < strlen(str); i++) {
        this->drawchar(str[i], x + i * 12, y, color);
    }
}


void lwin::linecircle_titlebar(u32 color, u32 center_x, u32 center_y, u32 rad) {
    double i = 0, angle = 0, x1 = 0, y1 = 0;

    for(i = 0; i < 360; i += 1) {
        angle = i;
        
        x1 = rad * math::cos(angle * math::PI / 180.00);
        y1 = rad * math::sin(angle * math::PI / 180.00);

        if (center_x + x1 > 0 && center_y + y1 > 0)
            this->putpixel_titlebar(center_x + x1, center_y + y1, color);
    }   
}

void lwin::fillrect_titlebar(u32 color, u32 start_x, u32 start_y, u32 width, u32 hight) {
    for (u32 y = start_y; y < hight + start_y; y++) {
        for (u32 x = start_x; x < width + start_x; x++) {
            this->putpixel_titlebar(x, y, color);
        }
    }
}

void lwin::fillcircle_titlebar(u32 color, u32 center_x, u32 center_y, u32 rad) {
    for (int w = 0; w < rad * 2; w++)
    {
        for (int h = 0; h < rad * 2; h++)
        {
            int dx = rad - w; // horizontal offset
            int dy = rad - h; // vertical offset

            if ((dx*dx + dy*dy) <= (rad * rad)) {
                this->putpixel_titlebar(center_x + dx, center_y + dy, color);
            }
        }
    }
}

void lwin::drawchar_titlebar(u32 ch, u32 x, u32 y, u32 color) {
    const unsigned char * gliph = built_in_font[ch - 32];
    
    for(int cy = 0; cy < 13; cy++) {
        u8 byte = gliph[cy];

        for (int i = 0; i < 8; i ++) {
            if (CHECK_BIT(byte, i)) {
                this->putpixel_titlebar((8 - i) + x, 13 - cy + y, color);
                //kout << "-";
            }
        }
    }
}

void lwin::drawstring_titlebar(const char* str, u32 x, u32 y, u32 color) {
    for (int i = 0; i < strlen(str); i++) {
        this->drawchar_titlebar(str[i], x + i * 12, y, color);
    }
}