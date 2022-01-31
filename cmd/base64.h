#ifndef __BASE64_H__
#define __BASE64_H__

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

extern char *base64_decode(const char*,char **);

extern char *base64_encode(const char*,char **);

#endif