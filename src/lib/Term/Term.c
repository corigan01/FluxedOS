#include "Term.h"

//index for video buffer array
uint32 vga_index;

//counter to store new lines
uint32 next_line_index = 1;

int buffer_reset = 0;

uint16* vga_buffer = (uint16*)VGA_ADDRESS;

uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

//clear video buffer array
void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  for(int i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
  }
  next_line_index = 1;
  vga_index = 0;
}

//initialize vga buffer
void init_vga(uint8 fore_color, uint8 back_color)
{
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
}

/*
increase vga_index by width of row(80)
*/
void print_new_line()
{
  if (next_line_index > 24) {
    clear_screen();
    return;
  }
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;

  buffer_reset = 0;
}

//assign ascii character to video buffer
void print_char(char ch, int Fcolor, int Bcolor, int charback)
{
  if (ch == '\n') {
    print_new_line();
    return;
  }
  if (ch == '\r') {
    print_new_line();
    return;
  }

  vga_index = vga_index - charback;
  vga_buffer[vga_index] = vga_entry(ch, Fcolor, Bcolor);

  if (charback == 0)
    vga_index++;
}


uint32 strlen(const char* str)
{
  uint32 length = 0;
  while(str[length] != '\0')
    length++;
  return length;
}

uint32 digit_count(int num)
{
  uint32 count = 0;
  if(num == 0)
    return 1;
  while(num > 0){
    count++;
    num = num/10;
  }
  return count;
}

void itoa(int num, char *number)
{
  int dgcount = digit_count(num);
  int index = dgcount - 1;
  char x;
  if(num == 0 && dgcount == 1){
    number[0] = '0';
    number[1] = '\0';
  }else{
    while(num != 0){
      x = num % 10;
      number[index] = x + '0';
      index--;
      num = num / 10;
    }
    number[dgcount] = '\0';
  }
}

//print string by calling print_char
void print_string(char *str, int Fcolor, int Bcolor)
{
  uint32 index = 0;
  while(str[index]){
    print_char(str[index], Fcolor, Bcolor, 0);
    index++;
  }
}

//print int by converting it into string
//& then printing string
void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num, g_fore_color, g_back_color);
}


void print_hold_int(int num) {
  if (buffer_reset == 0)
    buffer_reset = vga_index;
  
  vga_index = buffer_reset;

  char str_num[digit_count(num)+1];
  itoa(num, str_num);

  uint32 index = 0;
  while(str_num[index]){
    vga_buffer[vga_index] = vga_entry(str_num[index], g_fore_color, g_back_color);
    
    vga_index++;
    index++;
  }

}


void clear_screen() {
  clear_vga_buffer(&vga_buffer, WHITE, BLACK);
  for (int i = 0; i < vga_index; i++) {
    for (int e = 0; e < 80; e++) {
      print_char(' ', WHITE, BLACK, 0);
    }
  }
  clear_vga_buffer(&vga_buffer, WHITE, BLACK);
}


int get_vga_index() {
  return vga_index;
}

int get_line_index() {
  return next_line_index - 1;
}

void int_itoa(uint8_t *buf, uint32_t base, uint32_t d) {
   uint8_t *p = buf;
   uint8_t *p1, *p2;
   uint32_t ud = d;
   uint32_t divisor = 10;

   if(base == 'd' && d < 0) {
       *p++ = '-';
       buf++;
       ud = -d;
   } else
     if (base == 'x')
         divisor = 16;

   do{
       uint32_t remainder = ud % divisor;
       *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
   } while (ud /= divisor);

   *p = 0;
   p1 = buf;
   p2 = p - 1;
   while (p1 < p2) {
     uint8_t tmp = *p1;
     *p1 = *p2;
     *p2 = tmp;
     p1++;
     p2--;
   }
}


void printf(const char* format, ...) {
    uint8_t **arg = (uint8_t **) &format;
    uint8_t c;
    uint8_t buf[20];

    arg++;

    while((c = *format++) != 0) {
        if (c != '%')
            print_char (c, WHITE, BLACK, 0);
        else {
            uint8_t *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0') {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9') {
                pad = c - '0';
                c = *format++;
            }

            switch (c) {
            case 'X':
            case 'd':
            case 'u':
            case 'x':
                int_itoa (buf, c, *((int *) arg++));
                p = buf;
                goto string;
                break;
            case 's':
                p = *arg++;
                if (! p)
                p = (uint8_t*)"(null)";
    string:
                for (p2 = p; *p2; p2++);
                for (; p2 < p + pad; p2++)
                print_char (pad0 ? '0' : ' ', WHITE, BLACK, 0);
                while (*p)
                print_char (*p++, WHITE, BLACK, 0);
                break;
            default:
                print_char (*((int *) arg++), WHITE, BLACK, 0);
                break;
            }
            }
        }
}