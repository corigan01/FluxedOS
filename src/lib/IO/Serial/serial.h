#ifndef SERIAL__H__
#define SERIAL__H__

#include "../port.h"
#include "../../core/core.h"


enum SerialType {
        S_COM1 = 0x3f8,
        S_COM2,
        S_COM3,
        S_COM4
    };



class Serial {
    public:

    Serial();
    ~Serial();


    void outc(char byte);
    void outs(char * b);
    char in ();



    private:

    int received();
    int transmitEmpty();
    
    

};

void RawSerialoutc(char c);






#endif // !SERIAL__H__