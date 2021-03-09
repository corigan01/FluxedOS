#include "core.h"
#include "../Term/Term.h"

EXTNC_ void die() {
    clear_screen();
    init_vga(RED, WHITE);

    print_string("System Died! line called : ", RED, WHITE);
    print_int(__LINE__);

    while(1) {
        asm volatile ("nop");
    }
}


#define stringizer(xx) #xx
#define coriCase(xy, zz) case xy: \
asm volatile(stringizer(int zz)); \
break; 

EXTNC_ void ThrowISR(int n) {
    switch (n)
    {
    coriCase (0, $0x00);
    coriCase (1, $0x01);
    coriCase (2, $0x02);
    coriCase (3, $0x03);
    coriCase (4, $0x04);
    coriCase (5, $0x05);
    coriCase (6, $0x06);
    coriCase (7, $0x07);
    coriCase (8, $0x08);
    coriCase (9, $0x09);

    coriCase (10, $0x0A);
    coriCase (11, $0x0B);
    coriCase (12, $0x0C);
    coriCase (13, $0x0D);
    coriCase (14, $0x0E);
    coriCase (15, $0x0F);
    coriCase (16, $0x10);
    coriCase (17, $0x11);
    coriCase (18, $0x12);
    coriCase (19, $0x13);

    coriCase (20, $0x14);
    coriCase (21, $0x15);
    coriCase (22, $0x16);
    coriCase (23, $0x17);
    coriCase (24, $0x18);
    coriCase (25, $0x19);
    coriCase (26, $0x1A);
    coriCase (27, $0x1B);
    coriCase (28, $0x1C);
    coriCase (29, $0x1D);

    coriCase (30, $0x1E);
    coriCase (31, $0x1F);


    default:
        break;
    }
}