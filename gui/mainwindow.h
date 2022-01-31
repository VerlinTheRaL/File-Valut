#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QDir>

#include "user.h"
#include "comm.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
using namespace std;


class MainWindow: public QWidget
{
    Q_OBJECT

private:
    // widgets
    QTreeWidget *file_tree;
    QPushButton *refresh_button;
    QPushButton *input_button;
    QPushButton *output_button;
    QPushButton *delete_button;
    QPushButton *directory_button;

    // layouts
    QHBoxLayout *header_layout;
    QVBoxLayout *global_layout;

    // logic
    string current_path;
    string root_path;

    // modules
    User *user;
    KernelComm *netlinker;

    string parseFileSize(size_t);
    string parseFileTime(time_t);
    string FindPath(QTreeWidgetItem*);
    QString CurrentDir();
    bool isDirectory(mode_t);

public slots:
    void listDirectory(QTreeWidgetItem*, int);
    void updateFileTree();
    void InputFile();
    void OutputFile();
    void DeleteFile();
    void CreateDirectory();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setLayouts();
    void show();
   
};

#endif