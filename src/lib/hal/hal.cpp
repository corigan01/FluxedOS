#include "hal.h"

void SYS::reset() {
    uint8_t good = 0x02;
    while (good & 0x02)
        good = port_byte_in(0x64);
    port_byte_out(0x64, 0xFE);
    HALT;
}

// Just an emulator shutdown
void SYS::shutdown() {
    port_word_out(0xB004, 0x2000);
    port_word_out(0x604, 0x2000);
    port_word_out(0x4004, 0x3400);
}