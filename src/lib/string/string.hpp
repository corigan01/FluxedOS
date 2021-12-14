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

#include <lib/vector/KernelVector.hpp>
#include <lib/core/core.h>
#include <lib/assert/assert.hpp>
#include <System/kout/kout.hpp>
#include <lib/StanderdOperations/Operations.hpp>

// String is just a vector with type "const char*" with additional helpfulness regarding strings
class String : public K_Vector<char> {
    public:

    // Standered constructors
    String();
    String(char* str);
    String(const char* str);
    
    ~String();
    
    // Standered operations that string must support
    void operator= (char* str);
    void operator= (const char* str);
    void operator= (String str);

    void operator+= (char str);
    void operator+= (char* str);
    void operator+= (const char* str);
    void operator+= (String str);

    String operator+ (char* str);
    String operator+ (const char* str);
    String operator+ (String str);

    // to_string's for each type of thing we want to stringify
    String to_string(char i);
    String to_string(int i);
    String to_string(i32 i);
    String to_string(i16 i);
    String to_string(i8 i);

    const char * c_str();
    char*       cc_str();

    // may work on a find command because it can be helpful when programming complext things with string later down the line
    i32 find(String str);

    
    
};