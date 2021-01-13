#include "displayout.h"

char* D_debug[4] = {"INFO", "DEBUG", "ERROR", "WARNING"};
char* D_part[3] = {"Kernel", "External", "Unknown"};
int color_R[4] = {WHITE, BLUE, RED, YELLOW};


void out(int enu, int core_part, String str) {
    print_string("[ ", color_R[enu]);
    print_string(D_debug[enu], color_R[enu]);
    print_string(" - ", color_R[enu]);
    print_string(D_part[core_part], color_R[enu]);
    print_string(" ] --> ", color_R[enu]);

    for (int i = 0; i < str.size(); i++) {
        char ch = str[i];

        switch (ch)
        {
        case '\n':
            print_new_line();
            break;
        case '\t':
            for (int e = 0; e < TAB_SPACE; e++) 
                print_char(' ');
            break;
        default:
            print_char(ch,  color_R[enu]);
            break;
        }
    }

    print_new_line();

}
