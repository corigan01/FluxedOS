#ifndef STRING_KERNAL
#define STRING_KERNAL KERNAL_NEEDED

#include "../core/core.h"


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



#endif // !String
