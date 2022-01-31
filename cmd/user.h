#ifndef __USER_H__
#define __USER_H__

#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct passwd * get_user();
char* check_user(int);

#endif