#include "serial.h"
#include "../../compare/compare.h"

SerialType st;

Serial::Serial() {
    st = S_COM1;

    port_byte_out(st + 1, 0x00);
    port_byte_out(st + 3, 0x80);
    port_byte_out(st + 0, 0x03);
    port_byte_out(st + 1, 0x00);
    port_byte_out(st + 3, 0x03);
    port_byte_out(st + 2, 0xC7);
    port_byte_out(st + 4, 0x0B);
}

Serial::~Serial() {
    this->outs("Serial Decon\n");
}

void Serial::outc(char byte) {
    while (this->transmitEmpty() == 0);
    port_byte_out(st , byte);
}

void Serial::outs(char* s) {
    for (int i = 0; i < strlen(s); i++) {
        while (this->transmitEmpty() == 0);
        port_byte_out(st , s[i]);
    }
    
}

char Serial::in () {
    while (this->received() == 0);
    return port_byte_in(st);
}

int Serial::received() {
    return port_byte_in(st + 5) & 1;
}

int Serial::transmitEmpty() {
    return port_byte_in(st + 5) & 0x20;
}
    
    
void RawSerialoutc(char c){
    while (port_byte_in(st + 5) & 0x20 == 0);
    port_byte_out(S_COM1 , c);
}

