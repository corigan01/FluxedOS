#ifndef DISPLAYOUT_H_
#define DISPLAYOUT_H_

#include "../core/core.h"
#include "../String/String.h"

enum D_DEBUG {
    D_INFO = 0,
    D_DEBUG,
    D_ERROR,
    D_WARNING,
    D_
};


class displayout {

public:
    displayout();
    ~displayout();

    void out(int enu, int core_part, String str);

private:



};

#endif // !DISPLAYOUT_H_



