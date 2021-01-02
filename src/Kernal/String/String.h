#ifndef STRING_KERNAL
#define STRING_KERNAL KERNAL_NEEDED

#include "../core/core.h"

class String
{
public:
	String();
	~String();

	const char* c_str() {
		return Data;
	}

	void operator=(const char* _str_i) {
		this->_Size = find_size(_str_i);

		this->Data = _str_i;
	}



	uint16_t size() {
		return this->_Size;
	}

private:

	size_t find_size(const char* _str_i) {
		size_t ret = 0;
		while (_str_i[ret] != 0)
			ret++;
		return ret;
	}

	const char* Data;
	int _Size;

};




#endif // !String
