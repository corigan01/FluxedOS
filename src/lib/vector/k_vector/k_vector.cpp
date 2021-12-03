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
K_Vector<T>::K_Vector(size_t loc) {
    
}

template <class T>
K_Vector<T>::K_Vector() {
    h = (D *) System::Memory::kmalloc(sizeof(T) * 1000);
}
        
template <class T>
void K_Vector<T>::ChangePointer(void* p) {
    h = (D *) p;
}

template <class T>
void K_Vector<T>::push_back(T data) {
    
    for (int i = 0; i < VectorSize ; i++) {
        if (h[i].DoesPoint == 0) {

            // Assign K_Vector
            h[i].PointsTo = EndOfVector;
            h[i].DoesPoint = 1;
            h[i].Data = data;
            EndOfVector++;

            //this->PrintVector();

            return;
        }
    }

    if (VectorSize * sizeof(T) >= VECTOR_MAX_SIZE) {
        //G_ERR(VGA::PRINT_STR("K_Vector at max size\n"));
        kout << "K_Vector at max size\n";
        return;
    }

    h[VectorSize].PointsTo = EndOfVector;
    h[VectorSize].DoesPoint = 1;
    h[VectorSize].Data = data;

    EndOfVector++;   
    VectorSize++;

    //this->PrintVector();
            
}

template <class T>
void K_Vector<T>::pop_back() {
    for (int i = 0; i < VectorSize; i++) {
        if (h[i].PointsTo == (EndOfVector - 1)) {
            

            h[i].DoesPoint = 0;
            EndOfVector--;
            
            return;
        }
    }
}

template <class T>
int K_Vector<T>::size() {
    return EndOfVector;
}

template <class T>
void K_Vector<T>::pop_at(size_t s) {
    for (int i = 0; i < VectorSize; i++) {
        if (h[i].PointsTo == s) {
            h[i].DoesPoint = 0;
            EndOfVector--;
            IncRemaining(i, -1);

            return;
        }
    }
}

template <class T>
void K_Vector<T>::insert_at(size_t s, T d) {
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

template <class T>
T K_Vector<T>::getat(uint32 s) {
    for (int i = 0; i < VectorSize; i++) {
        if (h[i].DoesPoint == 1 && h[i].PointsTo == s) return h[i].Data;

    }

    return h[-1].Data; // we could not find that value
}

template <class T>
T& K_Vector<T>::operator  [](size_t s) {
    for (int i = 0; i < VectorSize; i++) {
        if (h[i].DoesPoint == 1 && h[i].PointsTo == s) return h[i].Data;

    }

    return h[-1].Data; // we could not find that value
}

template <class T>
void K_Vector<T>::operator =(K_Vector s) {
    this->EndOfVector = s.EndOfVector;
    this->VectorSize  = s.VectorSize;
    this.h            = s.h;
}

template <class T>
void K_Vector<T>::empty() {

    VectorSize = 0;
    EndOfVector = 0;
}

template <class T>
void K_Vector<T>::IncRemaining(int adr, int inc) {
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

