/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#pragma once

#include "../core/core.h"

int     strcmp      (const char *s1, const char *s2) ;
int     strncmp     (const char *s1, const char *s2, size_t n);
int     memcmp      (const void *b1, const void *b2, size_t n);
int     stoi        (char* str);
int     strcpy_     (char *dst, const char *src);
int     strncmp_    (const char* s1, const char* s2, int c );

uint32  strlen      (const char* str);
uint32  digit_count (int num);

void    itoa        (int num, char *number);
void    strcat      (void *dest,const void *src);
void    memcpy      (void *dest, void *src, size_t n);

void*   memset      (void *dst,char val, int n);


char *  strstr      (const char *in, const char *str);
char *  strdup      (const char * src);
char *  strsep      (char **stringp, const char *delim);

#define INT_TO_STRING(name, in)    \
    char name[40];                 \
    itoa(in, name);

