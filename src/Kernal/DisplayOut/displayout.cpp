

#include "displayout.h"


displayout::displayout() {
    print_char('#');
}

displayout::~displayout() {

}

void displayout::out(int enu, int core_part, String str) {
    print_char('[', color_R[enu]);
    print_string(D_debug[enu], color_R[enu]);
    print_string(" - ", color_R[enu]);
    print_string(D_part[enu], color_R[enu]);
    print_string("] --> ", color_R[enu]);

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


