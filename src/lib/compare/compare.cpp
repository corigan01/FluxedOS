#include "compare.h"

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
    char * end = (char*)dest + strlen((const char*)dest);
    memcpy((char*)end,(char*)src,strlen((char*)src));
    end = end + strlen((char*)src);
    *end = '\0';
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
    int len = strlen(src) + 1;
    char * dst = (char*)malloc(len);
    memcpy(dst, (void*)src, len);
    return dst;
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

list_t * str_split(const char * str, const char * delim, unsigned int * numtokens) {
    list_t * ret_list = LIST::create();
    char *s = strdup(str);
    char *token, *rest = s;
    while ((token = strsep(&rest, delim)) != NULL) {
        if(!strcmp(token, ".")) continue;
        if(!strcmp(token, "..")) {
            if(LIST::size(ret_list) > 0) LIST::pop(ret_list);
            continue;
        }
        LIST::push(ret_list, strdup(token));
        if(numtokens) (*numtokens)++;
    }
    free(s);
    return ret_list;
}

char * list2str(list_t * list, const char * delim) {
    char * ret = (char*)malloc(256);
    memset(ret, 0, 256);
    int len = 0, ret_len = 256;
    while(LIST::size(list)> 0) {
        char * temp = (char*)LIST::pop(list)->val;
        int len_temp = strlen(temp);
        if(len + len_temp + 1 + 1 > ret_len) {
            ret_len = ret_len * 2;
            ret = (char*)malloc(sizeof(ret) * ret_len);
            len = len + len_temp + 1;
        }
        strcat(ret, delim);
        strcat(ret, temp);
    }
    return ret;
}