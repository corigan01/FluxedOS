#include "String.h"
#include "../mem/mem.h"
#include "../list/list.h"
#include "../compare/compare.h"

String::String()
{
}

String::String(const char* _str_i) {
		this->_Size = find_size(_str_i);

		this->Data = _str_i;
}

const char* String::c_str() {
		return Data;
}

char * String::cs_ctr() {
	return (char*)Data;
}


void String::operator=(const char* _str_i) {
	if (_str_i != "")
		this->_Size = find_size(_str_i);
	else
		this->_Size = 0;
	

	this->Data = _str_i;
}

void String::operator=(String *str) {
    this->_Size = str->_Size;
    this->Data  = str->Data;
}

String String::operator+(String str) {
    char* s = (char*)this->c_str();
    int ind = this->size();
    for (int i = 0; i < str.size(); i ++) {
         s[ind + i] = str.Data[i];
    }
    return String(s);
}

char String::operator[](int i) {
	return Data[i];
}

uint16_t String::size() {
		return this->_Size;
}

size_t String::find_size(const char* _str_i) {
		size_t ret = 0;
		while (_str_i[ret] != 0)
			ret++;
		return ret;
}

bool String::operator==(const char* _str_i) {
	if (this->Data == _str_i) {
		return true;
	}
	return false;

}

bool String::operator==(String *str) {
	if (this->Data == str->Data) {
		return true;
	}
	return false;


}

String::~String()
{
	//delete this->Data;
	//delete &this->_Size;
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