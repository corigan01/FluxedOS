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

#include <lib/core/core.h>
#include "string.hpp"

// TODO: RE-MAKE STRING
// String has been causing problems, so i just deleted it because it needs to be redone
String::String() {

}
String::String(char* str) {

}
String::String(const char* str) {

}

String::~String() {
}

// Standered operations that string must support
void String::operator= (char* str) {

}
void String::operator= (const char* str){

}
void String::operator= (String str) {
    
}

void String::operator+= (char str) {

}

void String::operator+= (char* str) {

}
void String::operator+= (const char* str) {

}
void String::operator+= (String str) {

}

String String::operator+ (char* str) {

}
String String::operator+ (const char* str) {

}
String String::operator+ (String str) {

}

// to_string's for each type of thing we want to stringify
String String::to_string(char i) {

}
String String::to_string(int i) {

}
String String::to_string(u32 i) {

}
String String::to_string(u16 i) {

}
String String::to_string(u8 i) {

}

// may work on a find command because it can be helpful when programming complext things with string later down the line
u32 String::find(String str) {

}

const char * String::c_str() {

}

char* String::cc_str() {

}
