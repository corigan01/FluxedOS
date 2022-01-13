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


String::String() {

}
String::String(char* str) {
    this->operator=(str); 
}
String::String(const char* str) {
    this->operator=(str); 
}

String::~String() {
    // vector should handle this
    //this->free_pointer();
}

// Standered operations that string must support
void String::operator= (char* str) {
    empty();
	for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		this->push_back(e);
	}
}
void String::operator= (const char* str){
    empty();
	for (u32 i = 0; i < strlen(str); i++) {
        char e = str[i];
        kout << e << endl;
		this->push_back(e);
	}
}
void String::operator= (String str) {
    
}

void String::operator+= (char str) {
    kout << str << " ahhhh " << endl;
    this->push_back(str);
}

void String::operator+= (char* str) {
    for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		this->push_back(e);
	}
}
void String::operator+= (const char* str) {
    for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		this->push_back(e);
	}
}
void String::operator+= (String str) {
    for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		this->push_back(e);
	}
}

String String::operator+ (char* str) {
    String rstr = *this;

    for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		rstr += e;
	}

    return rstr;
}
String String::operator+ (const char* str) {
    String rstr = *this;

    for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		rstr += e;
	}

    return rstr;
}
String String::operator+ (String str) {
    String rstr = *this;

    
    for (u32 i = 0; str[i] != 0; i++) {
        auto e = str[i];
		rstr += e;
	}

    return rstr;
}

// to_string's for each type of thing we want to stringify
String String::to_string(char i) {
    INT_TO_STRING(str, i);
    String rstr = str;
    return rstr;
}
String String::to_string(int i) {
    INT_TO_STRING(str, i);
    String rstr = str;
    return rstr;
}
String String::to_string(u32 i) {
    INT_TO_STRING(str, i);
    String rstr = str;
    return rstr;
}
String String::to_string(u16 i) {
    INT_TO_STRING(str, i);
    String rstr = str;
    return rstr;
}
String String::to_string(u8 i) {
    INT_TO_STRING(str, i);
    String rstr = str;
    return rstr;
}

// may work on a find command because it can be helpful when programming complext things with string later down the line
u32 String::find(String str) {

}

const char * String::c_str() {
    NO_INSTRUCTION;
    char * ch = (char*)System::Memory::kmalloc(sizeof(char) * this->size());
	int all = 0;

	for (int i = 0; i < size(); i++) {
        NO_INSTRUCTION;
		ch[all++] = operator[](i);
	}
    ch[all++] = '\0';

    kout << size() << "::";
    for (int i = 0; i < size(); i++) {
        kout << operator[](i);
    }
    kout << endl;

    NO_INSTRUCTION;
	return (const char *)ch;
}

char* String::cc_str() {
    char * ch = "";
	int all = 0;

	for (int i = 0; i < size(); i++) {
        NO_INSTRUCTION;
		ch[all++] = operator[](i);
	}
    ch[all++] = '\0';

	return ch;
}