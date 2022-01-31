#ifndef USER_H
#define USER_H

#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <QFile>

using namespace std;

class User {

private:
    struct passwd *pw;

public:
    User();
    int pid();
    char *pname();
    int set_password(char*);
    char *check_user(int);
    void reset_password(char*);

};

#endif