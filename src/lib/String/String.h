#ifndef STRING_KERNAL
#define STRING_KERNAL KERNAL_NEEDED

#include "../core/core.h"
#include "../list/list.h"
#include "../Vector/vector.h"
#include "../compare/compare.h"


class string : public Vector<char> {
public:

	string();
	~string();

	void operator =(const char * s); 
	void operator =(string s);
	void operator =(char s);
	void operator =(Vector<char> s);
	void operator =(char * s);
	
	Vector<string> split(char * c);

	const char * c_str();
	char * cc_str();

};



#endif // !String
