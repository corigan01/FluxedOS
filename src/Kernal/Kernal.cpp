#include "Kernal.h"


extern "C"
int kernal_entry() {
	//first init vga with fore & back colors
    init_vga(WHITE, BLACK);

    print_string("FLUXED OS -------");
    print_new_line();

    char keystore = 0;
    while(1) {
        char keycode = get_input_keycode();

        if (keystore != keycode) {

            if (keycode == KEY_ENTER) {
                print_new_line();
            }
            else {
                char ch = get_ascii_char(keycode);
                print_char(ch);
            }
            keystore = keycode;
        }
    }

    while(1){};
    return 0;
}

