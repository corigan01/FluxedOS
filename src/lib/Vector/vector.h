#ifndef VECTOR_H_
#define VECTOR_H_

#include "../core/core.h"


template <class T>
class Vector {
    T *h;
    int index = 0;

    public:

    void push_back(T data){
        h[index] = data;
        index++;
    }
    
    T operator[](int ind){
        if (ind > index || ind < 0) {
            //out(D_ERROR, KERNEL, "Index must be in range[]");
        }

        return h[ind];
    }

    int size() {
        return index;
    }

};




#endif // !VECTOR_H_
