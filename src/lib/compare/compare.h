#include "../core/core.h"
#include "../list/list.h"
#include "../String/String.h"

#ifndef COMPARE_H_
#define COMPARE_H_

int strcmp(const char *s1, const char *s2) ;

int strncmp(const char *s1, const char *s2, size_t n);

int memcmp(const void *b1, const void *b2, size_t n);

int stoi(char* str);

uint32 strlen(const char* str);

uint32 digit_count(int num);

void itoa(int num, char *number);

char * strstr(const char *in, const char *str);

int strcpy_(char *dst,const char *src);

void strcat(void *dest,const void *src);

int strncmp_( const char* s1, const char* s2, int c );

char * strdup(const char * src);

char *strsep(char **stringp, const char *delim);

list_t * str_split(const char * str, const char * delim, unsigned int * numtokens);

char * list2str(list_t * list, const char * delim);

#endif // !COMPARE_H_
