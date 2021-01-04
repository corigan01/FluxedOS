#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"

int KernStart() {
    init_vga(WHITE, BLACK);

    print_string("FLUXED OS ------- build ");
    print_int(BUILD);
    print_new_line();


    print_string("test");

    return 0;
}