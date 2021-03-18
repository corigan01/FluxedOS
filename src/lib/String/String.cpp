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

