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
#include <lib/SmartPointer/smartpointer.hpp>
#include <System/memory/MemorySizes.hpp>
#include <System/memory/kmemory.hpp>
#include <System/kout/kout.hpp>
#include <System/memory/pmm/pmm.hpp>

#define VECTOR_MAX_SIZE (4 _MB)

template <class T>
class K_Vector {
    protected:
        struct D { // This is the main data storage for the vector, it contains the accual content
            T Data; // Data Storage
            int PointsTo = 0; // The index this unit will point to 
            bool DoesPoint = 0; // This tells if this data has been popped 
        };
        struct DB { // This is the data vector which stores banks of content for allocating more memory when the vector fills up
            D *h;
            int MemoryBank = 0;
            int alloc = 0;
        };
        DB *content;
        u32 contentsize = 0;

        u32 VectorSize = 0; // how many alive entries there are
        u32 EndOfVector = 0; // how many total entries there are

    public:

        K_Vector(size_t loc);
        K_Vector();
        
        ~K_Vector();

        void ChangePointer(void* p);
        void free_pointer();

    public:

        void push_back(T data);
        void pop_back();
        void empty(); 

        void pop_at(size_t s);
        void insert_at(size_t s, T d);

        int size();

        T getat(uint32 s);
        T& operator  [](size_t s);
        void operator =(K_Vector s);


    private:

        // This will increment the remaining vector numbers depening on what they point to
        // [ =  =  =  =  =  =] 1, 2, 3, 4, 5, 6 --> 1, 2, [3], 4, 5, 6, 7
        void IncRemaining(int adr, int inc);
};

