#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>

#define COMP_MAX 50  
#define ispathsep(ch)   ((ch) == '/' || (ch) == '\\')
#define iseos(ch)       ((ch) == '\0')
#define ispathend(ch)   (ispathsep(ch) || iseos(ch))


// path normalization
char *normpath(char *out, const char *in);

// base64 encrypt/decrypt
extern char *base64_decode(const char*,char **);
extern char *base64_encode(const char*,char **);


#endif