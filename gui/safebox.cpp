#include <QtWidgets/QApplication>

#include <iostream>
#include "mainwindow.h"
#include "loginwindow.h"
using namespace std;



int main(int argc,char *argv[])
{
    QApplication a(argc,argv);
    LoginWindow login;
    login.show();
    return a.exec();
}
