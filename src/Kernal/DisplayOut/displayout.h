#ifndef DISPLAYOUT_H_
#define DISPLAYOUT_H_

#include "../String/String.h"
#include "../Term/Term.h"

#define TAB_SPACE 5

enum D_DEBUG {
    D_INFO = 0,
    D_DEBUG,
    D_ERROR,
    D_WARNING,
};

enum D_PART {
    KERNEL = 0,
    EXTERNAL,
    UNKNOWN,
};


void out(int enu, int core_part, String str);


#endif // !DISPLAYOUT_H_



