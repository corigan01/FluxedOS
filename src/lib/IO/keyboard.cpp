#include "Keyboard.h"



char get_input_keycode()
{
  char ch = 0;
  ch = port_byte_in(KEYBOARD_PORT);
  if(ch > 0)
    return ch;
  
  return ch;
}

char get_ascii_char(uint8 key_code)
{
  switch(key_code){
    case KEY_A : return 'a';
    case KEY_B : return 'b';
    case KEY_C : return 'c';
    case KEY_D : return 'd';
    case KEY_E : return 'e';
    case KEY_F : return 'f';
    case KEY_G : return 'g';
    case KEY_H : return 'h';
    case KEY_I : return 'i';
    case KEY_J : return 'j';
    case KEY_K : return 'k';
    case KEY_L : return 'l';
    case KEY_M : return 'm';
    case KEY_N : return 'n';
    case KEY_O : return 'o';
    case KEY_P : return 'p';
    case KEY_Q : return 'q';
    case KEY_R : return 'r';
    case KEY_S : return 's';
    case KEY_T : return 't';
    case KEY_U : return 'u';
    case KEY_V : return 'v';
    case KEY_W : return 'w';
    case KEY_X : return 'x';
    case KEY_Y : return 'y';
    case KEY_Z : return 'z';
    case KEY_1 : return '1';
    case KEY_2 : return '2';
    case KEY_3 : return '3';
    case KEY_4 : return '4';
    case KEY_5 : return '5';
    case KEY_6 : return '6';
    case KEY_7 : return '7';
    case KEY_8 : return '8';
    case KEY_9 : return '9';
    case KEY_0 : return '0';
    case KEY_MINUS : return '-';
    case KEY_EQUAL : return '=';
    case KEY_SQUARE_OPEN_BRACKET : return '[';
    case KEY_SQUARE_CLOSE_BRACKET : return ']';
    case KEY_SEMICOLON : return ';';
    case KEY_BACKSLASH : return '\\';
    case KEY_COMMA : return ',';
    case KEY_DOT : return '.';
    case KEY_FORESLHASH : return '/';
    case KEY_SPACE : return ' ';
    case KEY_ENTER : return KEYCODE_ENTER;
    case KEY_BACKSPACE : return KEY_BACKSPACE;
    default : return 0;
  }
}




char __key_hold = ' ';
char getKeydown() {
  char key = get_ascii_char(get_input_keycode());
  
  if (__key_hold != key && key != NULL) {
      __key_hold = key;
      return key;

  }
  
  __key_hold = key;

  return NULL;
}

char getKeyCodedown() {
  char key = (get_input_keycode());
  
  if (__key_hold != key && key != NULL) {
      __key_hold = key;
      return key;

  }
  
  __key_hold = key;

  return NULL;
}