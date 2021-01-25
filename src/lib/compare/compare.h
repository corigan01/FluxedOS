#include "../core/core.h"
#ifndef COMPARE_H_
#define COMPARE_H_

int strcmp(const char *s1, const char *s2) ;

int strncmp(const char *s1, const char *s2, size_t n);

int memcmp(const void *b1, const void *b2, size_t n);

#endif // !COMPARE_H_
