#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"


int KernStart() {
    init_vga(WHITE, BLACK);

    print_string("FLUXED OS ------- build ", GREEN);
    print_int(BUILD);
    print_char('!', GREEN);
    print_new_line();

    String str;
    str = "dfsdf";
    str.c_str();

    displayout dis;
    dis.out(D_ERROR, KERNEL, "OUTPUT");

    print_new_line();
    print_string(" ------ OS FINISHED ------ ", RED);

    return 0;
}