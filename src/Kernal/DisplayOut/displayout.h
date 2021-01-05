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

extern "C++"
class displayout {

public:
    displayout();
    ~displayout();

    void out(int enu, int core_part, String str);

private:

    char* D_debug[4] = {"INFO", "DEBUG", "ERROR", "WARNING"};
    char* D_part[3] = {"Kernel", "External", "Unknown"};

    int color_R[4] = {WHITE, BLUE, RED, YELLOW};


};

#endif // !DISPLAYOUT_H_



