#ifndef STRING_KERNAL
#define STRING_KERNAL KERNAL_NEEDED

#include "../core/core.h"
#include "../list/list.h"


class String
{
public:
	String();
	String(const char* _str_i);
	~String();

	const char* c_str();
	char *cs_ctr();

	uint16_t size();

	void operator=(const char* _str_i);
	void operator=(String *str);
	char operator[](int i);
	
	bool operator==(const char* _str_i);
	bool operator==(String *str);
	
	String operator+(String str);

private:

	size_t find_size(const char* _str_i);

	const char* Data = "";
	int _Size;

};

int stoi(char* str);
uint32 strlen(const char* str);
uint32 digit_count(int num);
void itoa(int num, char *number);
char * strstr(const char *in, const char *str);
int strcpy_(char *dst,const char *src);


void strcat(void *dest,const void *src);
int strncmp_( const char* s1, const char* s2, int c );
char * strdup(const char * src);

int strcpy_(char *dst,const char *src);


void strcat(void *dest,const void *src);

int strncmp_( const char* s1, const char* s2, int c );

char * strdup(const char * src);

char *strsep(char **stringp, const char *delim);
list_t * str_split(const char * str, const char * delim, unsigned int * numtokens);
char * list2str(list_t * list, const char * delim);

#endif // !String
