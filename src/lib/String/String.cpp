#include "String.h"
#include "../mem/mem.h"
#include "../list/list.h"
#include "../compare/compare.h"


string::string(){
	;
}
string::~string(){
	; 
}

void string::operator =(const char * s) {
	empty();
	for (;auto e = s++ != 0;) {
		this->push_back(e);
	}
}
void string::operator =(string s) {
	this->EndOfVector = s.EndOfVector;
	this->VectorSize  = s.VectorSize;
	this->h           = s.h;
}
void string::operator =(char s) {
	empty();
	this->push_back(s);
}
void string::operator =(Vector<char> s) {
	empty();
	for (int i = 0; i < s.size(); i++) {
		this->push_back(s[i]);
	}
}
void string::operator =(char * s) {
	empty();
	for (;auto e = s++ != 0;) {
		this->push_back(e);
	}
}

Vector<string> string::split(char * c) {

}

const char * string::c_str() {

}
char * string::cc_str() {

}