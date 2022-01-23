/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#include "Operations.hpp"

int strcmp(const char *s1, const char *s2) {
   while (*s1 != '\0' && *s2 != '\0'  && *s1 == *s2) {
      s1++;
      s2++;
   }
   return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
   while (n != 0 && *s1 != '\0' && *s2 != '\0'  && *s1 == *s2) {
      s1++;
      s2++;
      n--;
   }
   return n == 0 ? 0 : (*s1 - *s2);
}

int memcmp(const void *b1, const void *b2, size_t n) {
   uint8_t *p1 = (uint8_t*)b1;
   uint8_t *p2 = (uint8_t*)b2;
   while (n > 0 && *p1 == *p2) {
      p1++;
      p2++;
      n--;
   }
   return n == 0 ? 0 : (*p1 - *p2);
}

int stoi(char* str)
{
    // Initialize result
    int res = 0;
 
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
 
    
    return res;
}

uint32 strlen(const char* str)
{
  uint32 length = 0;
  while(str[length] != '\0')
    length++;
  return length;
}

uint32 digit_count(u32 num)
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

void itoa(u32 num, char *number)
{
  u32 dgcount = digit_count(num);
  u32 index = dgcount - 1;
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

char * strstr(const char *in, const char *str) {
    char c;
    uint32_t len;

    c = *str++;
    if (!c)
        return (char *) in;

    len = strlen(str);
    do {
        char sc;

        do {
            sc = *in++;
            if (!sc)
                return (char *) 0;
        } while (sc != c);
    } while (strncmp_(in, str, len) != 0);

    return (char *) (in - 1);
}
int strcpy_(char *dst,const char *src)
{
    int i = 0;
    while ((*dst++ = *src++) != 0)
        i++;
    return i;
}


void strcat(void *dest,const void *src)
{
    // been removed because it needs malloc
    //char * end = (char*)dest + strlen((const char*)dest);
    //memcpy((char*)end,(char*)src,strlen((char*)src));
    //end = end + strlen((char*)src);
    //*end = '\0';
}

int strncmp_( const char* s1, const char* s2, int c ) {
    int result = 0;

    while ( c ) {
        result = *s1 - *s2++;

        if ( ( result != 0 ) || ( *s1++ == 0 ) ) {
            break;
        }

        c--;
    }

    return result;
}

char * strdup(const char * src) {
    // been removed because it needs memcpy
    //int len = strlen(src) + 1;
    //char * dst = (char*)malloc(len);
    //memcpy(dst, (void*)src, len);
    //return dst;
}

char *strsep(char **stringp, const char *delim) {
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

void memcpy(void *dest, void *src, size_t n) 
{ 
   char *csrc = (char *)src; 
   char *cdest = (char *)dest; 
  
   for (int i=0; i<n; i++) 
       cdest[i] = csrc[i]; 
} 

void *memset(void *dst,char val, int n)
{
    char *temp = (char*)dst;
    for(;n != 0; n--) *temp++ = val;
    return dst;
}

