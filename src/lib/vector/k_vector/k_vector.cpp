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
#include <lib/vector/k_vector/k_vector.hpp>

template <class T> 
K_Vector<T>::~K_Vector() {
    kout << "K_VECTOR IS DECONSTRUCTED" << endl;
    free_pointer();
}

template <class T>
void K_Vector<T>::free_pointer() {
    for (int bank = 0; bank < contentsize; bank++) {
        auto ContentBank = &content[bank];
        System::Memory::kfree((void*)ContentBank);
    }
}

template <class T>
K_Vector<T>::K_Vector(size_t loc) {
    ChangePointer((void*)loc);
}

template <class T>
K_Vector<T>::K_Vector() {
    ChangePointer(System::Memory::kmalloc((sizeof(T) * 1000) + (sizeof(DB) * 10)));
    
}
        
template <class T>
void K_Vector<T>::ChangePointer(void* p) {
    u32* FirstMemoryPointer = (u32*)p;
    content = (DB*)FirstMemoryPointer;
    contentsize = 0;
    kout << "First Memory Pointer: " << kout.ToHex((u32)FirstMemoryPointer) << endl;
    
    FirstMemoryPointer += (sizeof(DB) * 10);

    D* temp_content = (D*)FirstMemoryPointer;
    content[contentsize].h = temp_content;
    content[contentsize++].MemoryBank = 0;

    VectorSize = 0;
    EndOfVector = 0;
}

template <class T>
void K_Vector<T>::push_back(T input) {
    insert_at(VectorSize, input);

}

template <class T>
void K_Vector<T>::pop_back() {
    pop_at(VectorSize - 1);
}

template <class T>
int K_Vector<T>::size() {
    return VectorSize;
}

template <class T>
void K_Vector<T>::pop_at(size_t s) {
    bool SuccessfulAlloc = false;
    // we dont find the address already so we have to create a new one
    for (int bank = 0; bank < contentsize; bank++) {
        auto ContentBank = &content[bank];
        
        for (int i = 0; i < ContentBank->alloc; i++) {
            // then we look through the data and see if we can find an full space
            auto data = &ContentBank->h[i];
            if (data->DoesPoint == true && data->PointsTo == s) {
                if (!SuccessfulAlloc) {
                    // we then remove that entry 
                    data->DoesPoint = false;
                    data->PointsTo = -1;
                    VectorSize--;
                    

                    SuccessfulAlloc = true;
                }
            }
            else {
                if (data->PointsTo >= s) {
                    data->PointsTo--;
                }
            }
        }
     
        

    }    
}

template <class T>
void K_Vector<T>::insert_at(size_t s, T d) {
    bool SuccessfulAlloc = false;
    // we dont find the address already so we have to create a new one
    for (int bank = 0; bank < contentsize; bank++) {
        auto ContentBank = &content[bank];

        // then we check if that bank is full or not
        if (ContentBank->alloc < 1000) {
            for (int i = 0; i < ContentBank->alloc; i++) {

                // then we look through the data and see if we can find an empty space
                auto data = &ContentBank->h[i];
                if (data->DoesPoint == false ) {
                    if (!SuccessfulAlloc) {
                        // we then fill that entry 
                        data->DoesPoint = true;
                        data->PointsTo = s;
                        data->Data = d;

                        VectorSize++;

                        SuccessfulAlloc = true;
                    }
                }
                else {
                    if (data->PointsTo >= s) {
                        data->PointsTo++;
                    }
                }
            }
        }
    }

    // alloc new bank here because we dont have any empty addresses
    if (!SuccessfulAlloc) {
        bool HasRoom = false;
        for (int bank = 0; bank < contentsize; bank++) {
            auto ContentBank = &content[bank];

            if (ContentBank->alloc < 1000) {
                HasRoom = true;

                ContentBank->h[ContentBank->alloc++] = {d, s, true};
                VectorSize++;
            }
        }
    }

    /*for (int bank = 0; bank < contentsize; bank++) {
        auto ContentBank = content[bank];

        
        for (int i = 0; i < ContentBank.alloc; i++) {

            // then we look through the data and see if we can find an empty space
            auto data = &ContentBank.h[i];
            
            kout << data->DoesPoint << " ------ " << data->PointsTo << endl;
            
        }
        
    }

    kout << endl << endl;*/
    
}

template <class T>
T K_Vector<T>::getat(uint32 s) {
    for (int bank = 0; bank < contentsize; bank++) {
        auto ContentBank = &content[bank];   

        for (int i = 0; i < ContentBank->alloc; i++) {
            auto data = ContentBank->h[i];

            if (data.DoesPoint == true && data.PointsTo == s)
                NO_INSTRUCTION;
                return data.Data;
            
        }
        
    }
}

template <class T>
T& K_Vector<T>::operator [](size_t s) {
    for (int bank = 0; bank < contentsize; bank++) {
        auto ContentBank = &content[bank];   

        for (int i = 0; i < ContentBank->alloc; i++) {
            auto data = &ContentBank->h[i];

            if (data->DoesPoint == true && data->PointsTo == s)
                return data->Data;
            
        }
        
    }

    ASSERT_NOT_REACHED("OUT OF BOUNDS!");
}

template <class T>
void K_Vector<T>::operator =(K_Vector s) {
    
}

template <class T>
void K_Vector<T>::empty() {
    for (int i = 0; size() != 0; i++) {
        pop_back();
    }
}

template <class T>
void K_Vector<T>::IncRemaining(int adr, int inc) {
    
}

