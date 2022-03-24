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
#include <lib/StanderdOperations/Operations.h>
#include <lib/assert/assert.hpp>
#include <System/memory/kmemory.hpp>

#define CAST_TO_SMART_POINTER(type, pointer) ((type*)((void*)pointer))

template <class T>
class SmartPtr {
    T* ptr;
public:
    SmartPtr(T* p) { ptr = p; }
    SmartPtr() { ptr = (T*)System::Memory::kmalloc(sizeof(T)); }
    ~SmartPtr() { System::Memory::kfree((void*)ptr); }

    T* rawptr() { return ptr; }

    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }

    void operator=(T* p) {
        System::Memory::kfree((void*)ptr);
        ptr = p;
    }

    bool operator==(SmartPtr<T> p) { return p.ptr == ptr; }
    bool operator==(T* p) { return p == ptr; }

};
