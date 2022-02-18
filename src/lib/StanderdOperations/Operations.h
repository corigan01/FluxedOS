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

#include <lib/core/core.h>

EXTNC_ int     strcmp      (const char *s1, const char *s2) ;
EXTNC_ int     strncmp     (const char *s1, const char *s2, size_t n);
EXTNC_ int     memcmp      (const void *b1, const void *b2, size_t n);
EXTNC_ int     stoi        (char* str);
EXTNC_ int     strcpy_     (char *dst, const char *src);
EXTNC_ int     strncmp_    (const char* s1, const char* s2, int c );

EXTNC_ uint32  strlen      (const char* str);
EXTNC_ uint32  digit_count (u32 num);
 
EXTNC_ void    itoa        (u32 num, char *number);
EXTNC_ void    strcat      (void *dest,const void *src);
EXTNC_ void    memcpy      (void *dest, void *src, size_t n);
 
EXTNC_ void*   memset      (void *dst,char val, int n);
 
 
EXTNC_ char *  strstr      (const char *in, const char *str);
EXTNC_ char *  strdup      (const char * src);
EXTNC_ char *  strsep      (char **stringp, const char *delim);

#define INT_TO_STRING(name, in)    \
    char name[40];                 \
    itoa(in, name);

