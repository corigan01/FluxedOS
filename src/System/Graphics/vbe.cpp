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

#include <System/Graphics/vbe.hpp>
#include <System/Clock/PIT/PIT.hpp>

using namespace System::Graphics;
            
u8* Buffer = (u8*)0xFFFFFF;
u32 rex = 0, rey = 0;

u32 pixelwidth = 1, pitch = 1;

void Driver::gxinit(void* buffer, u32 screen_x, u32 screen_y)  {
    rex = screen_x;
    rey = screen_y;

    Buffer = (u8*)buffer;
    pixelwidth = 1;
    pitch = 4;

    //fillcircle(0xFF0000, 100, 100, 100);
}

void Driver::putpixel(int x, int y, u32 color) {
    if (x < 0 || x >= rex || y < 0 || y >= rey) return;

    u32 where = (x * pitch) + (y * pitch * rex);
    Buffer[where] = color & 255;              // BLUE
    Buffer[where + 1] = (color >> 8) & 255;   // GREEN
    Buffer[where + 2] = (color >> 16) & 255;  // RED
}



void Driver::fillrect(u32 color, u32 start_x, u32 start_y, u32 width, u32 hight) {
    for (u32 y = start_y; y < hight + start_y; y++) {
        for (u32 x = start_x; x < width + start_x; x++) {
            putpixel(x, y, color);
        }
    }
}

void Driver::linecircle(u32 color, u32 center_x, u32 center_y, u32 rad) {
    double i = 0, angle = 0, x1 = 0, y1 = 0;

    for(i = 0; i < 360; i += 1) {
        angle = i;
        
        x1 = rad * math::cos(angle * math::PI / 180.00);
        y1 = rad * math::sin(angle * math::PI / 180.00);

        if (center_x + x1 > 0 && center_y + y1 > 0)
            putpixel(center_x + x1, center_y + y1, color);
    }   
}

void Driver::fillcircle(u32 color, u32 center_x, u32 center_y, u32 rad) {
    for (int w = 0; w < rad * 2; w++)
    {
        for (int h = 0; h < rad * 2; h++)
        {
            int dx = rad - w; // horizontal offset
            int dy = rad - h; // vertical offset

            if ((dx*dx + dy*dy) <= (rad * rad)) {
                putpixel(center_x + dx, center_y + dy, color);
            }
        }
    }
}

void Driver::drawchar(u32 ch, u32 x, u32 y, u32 color) {
    const unsigned char * gliph = built_in_font[ch - 32];

    for(int cy = 0; cy < 13; cy++) {
        u8 byte = gliph[cy];

        for (int i = 0; i < 8; i ++) {
            if (CHECK_BIT(byte, i)) {
                putpixel((8 - i) + x, 13 - cy + y, color);
            }
        }
    }
}

void Driver::drawstring(const char* str, u32 x, u32 y, u32 color) {
    for (int i = 0; i < strlen(str); i++) {
        drawchar(str[i], x + i * 12, y, color);
    }
}

Driver::GraphicsInfo Driver::getinfo() {
    Driver::GraphicsInfo info;
    info.width = rex;
    info.height = rey;
    info.pitch = pitch;
    info.pixelwidth = pixelwidth;
    info.buffer = (Buffer);
    return info;
}