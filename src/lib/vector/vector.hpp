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
class Vector {
    protected:
        struct D {
            T Data; // Data Storage
            int PointsTo = 0; // The index this unit will point to 
            int DoesPoint = 0; // This tells if this data has been popped 
        };
        D *h;

        int VectorSize = 0;
        int EndOfVector = 0;

    public:

        Vector(size_t loc) {
            
        }
        Vector() {
            h = (D *) System::Memory::pmm::ReservePage();
        }
        
        void ChangePointer(void* p) {
            h = (D *) p;
        }

        

        //~Vector() {
            // free the vector
            //free(h);
        //}

    public:

        void push_back(T data) {
            
            for (int i = 0; i < VectorSize ; i++) {
                if (h[i].DoesPoint == 0) {

                    // Assign Vector
                    h[i].PointsTo = EndOfVector;
                    h[i].DoesPoint = 1;
                    h[i].Data = data;
                    EndOfVector++;

                    //this->PrintVector();

                    return;
                }
            }

            if (VectorSize >= VECTOR_MAX_SIZE) {
                //G_ERR(VGA::PRINT_STR("Vector at max size\n"));
                return;
            }

            h[VectorSize].PointsTo = EndOfVector;
            h[VectorSize].DoesPoint = 1;
            h[VectorSize].Data = data;

            EndOfVector++;   
            VectorSize++;

            //this->PrintVector();
                 
        }

        void pop_back() {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].PointsTo == (EndOfVector - 1)) {
                    

                    h[i].DoesPoint = 0;
                    h[i].Data = NULL;
                    EndOfVector--;
                    
                    return;
                }
            }
        }

        int size() {
            return EndOfVector;
        }

        


        void pop_at(size_t s) {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].PointsTo == s) {
                    h[i].DoesPoint = 0;
                    h[i].Data = NULL;
                    EndOfVector--;
                    IncRemaining(i, -1);

                    return;
                }
            }
        }

        void insert_at(size_t s, T d) {

            // This is the easy way before we do the hard way
            // this will check if the value at the index does not point, and insert there
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].DoesPoint == false) {

                    IncRemaining(s - 1, 1);

                
                    
                    
                    h[i].DoesPoint = 1;
                    h[i].Data = d;
                    h[i].PointsTo = s;

                    EndOfVector++;

                    return;
               
                }
            }

            // This is the hard way, we now have to insert a new addr for that value
            
            
            
            IncRemaining(s - 1 , 1);
            
            h[VectorSize].DoesPoint = 1;
            h[VectorSize].PointsTo = s;
            h[VectorSize].Data = d;

            EndOfVector++;
            VectorSize++;

        }

        T getat(uint32 s) {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].DoesPoint == 1 && h[i].PointsTo == s) return h[i].Data;

            }

            return h[-1].Data; // we could not find that value
        }

        T& operator  [](size_t s) {
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].DoesPoint == 1 && h[i].PointsTo == s) return h[i].Data;

            }

            return h[-1].Data; // we could not find that value
        }

        void operator =(Vector s) {
            this->EndOfVector = s.EndOfVector;
            this->VectorSize  = s.VectorSize;
            this.h            = s.h;
        }


        void empty() {
            
            //memset(h, NULL, VECTOR_MAX_SIZE);

            VectorSize = 0;
            EndOfVector = 0;
        }

       

    private:

        // This will increment the remaining vector numbers depening on what they point to
        // [ =  =  =  =  =  =] 1, 2, 3, 4, 5, 6 --> 1, 2, [3], 4, 5, 6, 7
        void IncRemaining(int adr, int inc) {
            if (adr < 0) {
                for (int i = 0; i < VectorSize; i++) {
                    h[i].PointsTo += inc;
                }
                return;
            }
            for (int i = 0; i < VectorSize; i++) {
                if (h[i].PointsTo > adr) {
                    h[i].PointsTo += inc;
                }
            }
        }

       


};
