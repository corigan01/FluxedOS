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
    //kout << "K_VECTOR IS DECONSTRUCTED" << endl;
    this->delete_all();
    free_pointer();
}

template <class T>
void K_Vector<T>::free_pointer() {
    for (int bank = 0; bank < content_size; bank++) {
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
    ChangePointer(System::Memory::kmalloc((sizeof(T) * 1000) + (sizeof(data_base_t) * 10)));
    
}
        
template <class T>
void K_Vector<T>::ChangePointer(void* p) {
    free_pointer();

    u32* FirstMemoryPointer = (u32*)p;
    content = (data_base_t*)FirstMemoryPointer;
    content_size = 0;
    //kout << "First Memory Pointer: " << kout.ToHex((u32)FirstMemoryPointer) << endl;
    
    FirstMemoryPointer += (sizeof(data_base_t) * 10);

    auto* temp_content = (data_t*)FirstMemoryPointer;
    content[content_size].h = temp_content;
    content[content_size++].MemoryBank = 0;

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
    // we don't find the address already, so we have to create a new one
    for (int bank = 0; bank < content_size; bank++) {
        auto ContentBank = &content[bank];
        
        for (int i = 0; i < ContentBank->alloc; i++) {
            // then we look through the data and see if we can find a full space
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
    // we don't find the address already, so we have to create a new one
    for (int bank = 0; bank < content_size; bank++) {
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

    // alloc new bank here because we don't have any empty addresses
    if (!SuccessfulAlloc) {
        bool HasRoom = false;
        for (int bank = 0; bank < content_size; bank++) {
            auto ContentBank = &content[bank];

            if (ContentBank->alloc < 1000) {
                HasRoom = true;

                ContentBank->h[ContentBank->alloc++] = {d, s, true};
                VectorSize++;
            }
        }
    }
}

template <class T>
T K_Vector<T>::getat(uint32 s) {
    for (int bank = 0; bank < content_size; bank++) {
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
    for (int bank = 0; bank < content_size; bank++) {
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
K_Vector<T>& K_Vector<T>::operator =(K_Vector s) {
    // Basically delete our own class and start new
    delete_all();

    for (size_t i = 0; i < s.size(); i++) {
        this->push_back(s[i]);
    }
}

template <class T>
void K_Vector<T>::delete_all() {
    while (this->size() != 0) {
        pop_back();
    }
}

template<class T>
void K_Vector<T>::construct_pointer() {
    ChangePointer(System::Memory::kmalloc((sizeof(T) * 1000) + (sizeof(data_base_t) * 10)));
}

template<class T>
K_Vector<T>::K_Vector(K_Vector && vec)  noexcept {
    // Basically delete our own class and start new
    construct_pointer();
    delete_all();

    for (size_t i = 0; i < vec.size(); i++) {
        this->push_back(vec[i]);
    }
}

template<class T>
K_Vector<T>::K_Vector(const K_Vector & vec) {
    // Basically delete our own class and start new
    construct_pointer();
    delete_all();

    for (size_t i = 0; i < vec.size(); i++) {
        this->push_back(vec[i]);
    }
}


