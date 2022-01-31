#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    // display
    setWindowTitle("FileVault");
    setFixedSize(800, 600);
    this->move(250, 50);

    // logic
    user = new User;
    string p_name(user->pname()); 
    root_path = "/home/safebox/" + p_name + "/";
    netlinker = new KernelComm;

    // widgets
    refresh_button = new QPushButton(QString("refresh"), this);
    refresh_button->setFixedSize(80, 30);

    input_button = new QPushButton(QString("Move In"), this);
    input_button->setFixedSize(80, 30);

    output_button = new QPushButton(QString("Move Out"), this);
    output_button->setFixedSize(80, 30);

    delete_button = new QPushButton(QString("delete"), this);
    delete_button->setFixedSize(80, 30);

    directory_button = new QPushButton(QString("Create Dir"), this);
    directory_button->setFixedSize(80, 30);

    file_tree = new QTreeWidget(this);
    QStringList tree_header;
    tree_header << "Name" << "Size" << "Last Time";
    file_tree->setHeaderLabels(tree_header);
    file_tree->setColumnWidth(0, 300); 
    file_tree->setColumnWidth(1, 150);
    file_tree->setColumnWidth(2, 300);

    // layout
    this->setLayouts();

    // events
    connect(file_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),\
            this, SLOT(listDirectory(QTreeWidgetItem*, int)));
    connect(refresh_button, SIGNAL(released()), this, SLOT(updateFileTree()));
    connect(input_button, SIGNAL(released()), this, SLOT(InputFile()));
    connect(output_button, SIGNAL(released()), this, SLOT(OutputFile()));
    connect(delete_button, SIGNAL(released()), this, SLOT(DeleteFile()));
    connect(directory_button, SIGNAL(released()), this, SLOT(CreateDirectory()));
}

string MainWindow::FindPath(QTreeWidgetItem* item)
{
    stack<string> parentnames;
    QTreeWidgetItem *tmp = item;
    string full_path = root_path;
    while(tmp->parent() != NULL)
    {
        parentnames.push(tmp->parent()->text(0).toStdString());
        tmp = tmp->parent();
    }
    while(!parentnames.empty())
    {
        full_path += parentnames.top(); parentnames.pop();
        full_path += "/";
    }

    string item_name = item->text(0).toStdString();
    full_path += item_name;

    return full_path;
}

QString MainWindow::CurrentDir()
{
    QString path;
    if (file_tree->currentItem())
    {
        QTreeWidgetItem *CurrentItem = file_tree->currentItem();
        path = QString::fromStdString(FindPath(CurrentItem));
        QFileInfo FileInfo(path);
        if (FileInfo.isFile())
        {
            QStringList temp = path.split("/");
            path = temp[0];
            for (int i = 1; i < temp.size() - 1; ++i)
                path += "/" + temp[i];
        }
        path += "/";
    }
    else
        path = QString::fromStdString(root_path);
    return path;
}

void MainWindow::listDirectory(QTreeWidgetItem* item, int)
{
    if (item->child(0) != NULL)
        return;
    netlinker->send_pid();
    // get fullpath
    string full_path = FindPath(item);
    
    // check if item is a directory
    struct stat stat_buf; 
    stat(full_path.c_str(), &stat_buf);
    if (!isDirectory(stat_buf.st_mode))
        return;
    full_path += "/";
    

    DIR* dp; struct dirent* ep;

    // traverse directory
    dp = opendir(full_path.c_str());
    if (dp == NULL)
        return;

    int file_count = 0;

    while ((ep=readdir(dp))!= NULL)
    {
        if (strcmp(".", ep->d_name) == 0 || strcmp("..", ep->d_name) == 0)
            continue;

        // new item
        QTreeWidgetItem *child_file_node = new QTreeWidgetItem();
        
        // set name
        string file_name(ep->d_name); child_file_node->setText(0, QString::fromStdString(file_name));
        string abs_path = full_path + file_name;

        // set icon
        if (ep->d_type == 4)
        {
            QIcon icon("res/folder.png");
            child_file_node->setIcon(0, icon);
        }
        else if (ep->d_type == 8)
        {
            QIcon icon("res/file.png");
            child_file_node->setIcon(0, icon);
        }

        // get file info
        struct stat stat_buf; 
        stat(abs_path.c_str(), &stat_buf);

        // set size
        child_file_node->setText(1, QString::fromStdString(parseFileSize(stat_buf.st_size)));
        // set time
        child_file_node->setText(2, QString::fromStdString(parseFileTime(stat_buf.st_mtime)));

        item->addChild(child_file_node);
        file_count++;
    }
    closedir(dp); 

    if(file_count == 0)
    {
        QMessageBox::information(this,"Info", "Empty directory.");
    }

}

void MainWindow::show()
{
    // overload QWidget::show()
    updateFileTree();
    QWidget::show();
}


void MainWindow::updateFileTree()
{
    file_tree->clear();
    netlinker->send_pid();
    DIR* dp; struct dirent* ep;

    // traverse directory
    dp = opendir(root_path.c_str());
    if (dp != NULL)
    {
        while ((ep=readdir(dp))!= NULL)
        {
            if (strcmp(".", ep->d_name) == 0 || strcmp("..", ep->d_name) == 0)
                continue;

            // new item
            QTreeWidgetItem *file_node = new QTreeWidgetItem(file_tree);
            
            // set name
            string file_name(ep->d_name); file_node->setText(0, QString::fromStdString(file_name));
            string full_path = root_path + file_name;

            // set icon
            if (ep->d_type == 4)
            {
                QIcon icon("res/folder.png");
                file_node->setIcon(0, icon);
            }
            else if (ep->d_type == 8)
            {
                QIcon icon("res/file.png");
                file_node->setIcon(0, icon);
            }

            // get file info
            struct stat stat_buf; 
            stat(full_path.c_str(), &stat_buf);

            // set size
            file_node->setText(1, QString::fromStdString(parseFileSize(stat_buf.st_size)));
            // set time
            file_node->setText(2, QString::fromStdString(parseFileTime(stat_buf.st_mtime)));  

        }
        closedir(dp);
    }

}


bool MainWindow::isDirectory(mode_t mode)
{
    int res = mode & S_IFDIR;
    return (res == S_IFDIR) ? true:false;
}

string MainWindow::parseFileSize(size_t size)
{
    float ret_k = size / 1024.0;
    float ret_m = ret_k / 1024.0;
    float ret_g = ret_m / 1024.0;

    string size_ret;
    if (int(ret_g) != 0)
    {
        stringstream stream; stream << fixed << setprecision(2) << ret_g;
        size_ret = stream.str() + "GB";
    }
    else if (int(ret_m) != 0)
    {
        stringstream stream; stream << fixed << setprecision(2) << ret_m;
        size_ret = stream.str() + "MB";
    }
    else
    {
        stringstream stream; stream << fixed << setprecision(2) << ret_k;
        size_ret = stream.str() + "KB";
    }
    return size_ret;

}

string MainWindow::parseFileTime(time_t time)
{
    struct tm time_buf; char time_str[80];
    localtime_r(&time, &time_buf);
    strftime(time_str, sizeof(time_str), "%c", &time_buf);
    string time_ret(time_str);
    return time_ret;
}

void MainWindow::InputFile()
{
    netlinker->send_pid();
    QString path;

    path = CurrentDir();
    //定义文件对话框类
	QFileDialog *fileDialog = new QFileDialog(this);
	//定义文件对话框标题
	fileDialog->setWindowTitle(QStringLiteral("选中文件"));
	//设置默认文件路径
	fileDialog->setDirectory(".");
	//设置文件过滤器
	// fileDialog->setNameFilter(tr("File(*.*)"));
	//设置可以选择多个文件,默认为只能选择一个文件
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	//设置视图模式
	fileDialog->setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (fileDialog->exec()) {
		fileNames = fileDialog->selectedFiles();
    }
    for (int i = 0; i < fileNames.size(); i++) {
        QString old_path = fileNames[i];
        QStringList temp = old_path.split("/");
        QString new_path = path + temp[temp.size() - 1];
        QFile::rename(old_path,new_path);
    }
    updateFileTree();
}

void MainWindow::OutputFile()
{
    netlinker->send_pid();
    if (file_tree->currentItem())
    {
        QTreeWidgetItem *CurrentItem = file_tree->currentItem();
        QFileDialog *fileDialog = new QFileDialog(this);
        fileDialog->setWindowTitle(QStringLiteral("选中文件"));
        fileDialog->setDirectory(".");
        fileDialog->setFileMode(QFileDialog::Directory);
        fileDialog->setViewMode(QFileDialog::Detail);
        QStringList fileNames;
        if (fileDialog->exec()) {
            fileNames = fileDialog->selectedFiles();
            QString old_path = QString::fromStdString(FindPath(CurrentItem));
            // QString strText = CurrentItem->text(0);
            // QMessageBox::information(this,"Info", strText);
            QString new_path = fileNames[0] + "/" + CurrentItem->text(0);
            // QMessageBox::information(this,"Info", QFile::rename(old_path,new_path));
            if (!QFile::rename(old_path,new_path))
                QMessageBox::warning(this,"Warning", "Directory already exists!");
            updateFileTree();
        }
    }

    else
    {
        QMessageBox::warning(this,"Warning", "Please select a file or a directory!");
    }
}

void MainWindow::DeleteFile()
{
    netlinker->send_pid();
    if (file_tree->currentItem())
    {
        QMessageBox::StandardButton result = QMessageBox::information(NULL, "Caution", "File or directory will be deleted permanently. Continue?",QMessageBox::Yes|QMessageBox::No);
        if (result == QMessageBox::Yes)
        {
            QTreeWidgetItem *CurrentItem = file_tree->currentItem();
            QString path = QString::fromStdString(FindPath(CurrentItem));
            QFileInfo FileInfo(path);
            if (FileInfo.isFile())
                QFile::remove(path);
            else if (FileInfo.isDir())
            {
                QDir qDir(path);
                qDir.removeRecursively();
            }
            updateFileTree();
        }
    }

    else
    {
        QMessageBox::warning(this,"Warning", "Please select a file or a directory!");
    }
}

void MainWindow::CreateDirectory()
{
    netlinker->send_pid();
    bool isOK;
    QString text = QInputDialog::getText(this, "Create Directory", "Please input directory name", QLineEdit::Normal, "", &isOK);
    if (isOK)
    {
        QString path;
        path = CurrentDir();

        QDir dir;
        dir.cd(path);
        if(!dir.exists(text))//判断需要创建的文件夹是否存在
        {
            dir.mkdir(text); //创建文件夹
        }
        else
            QMessageBox::warning(this,"Warning", "Directory already exists!");
        updateFileTree();
    }
}

void MainWindow::setLayouts()
{
    header_layout = new QHBoxLayout();
    header_layout->addWidget(refresh_button);
    header_layout->addWidget(input_button);
    header_layout->addWidget(output_button);
    header_layout->addWidget(delete_button);
    header_layout->addWidget(directory_button);
    header_layout->addStretch(0);

    global_layout = new QVBoxLayout();
    global_layout->addLayout(header_layout);
    global_layout->addWidget(file_tree);

    this->setLayout(global_layout);
}


MainWindow::~MainWindow()
{
    delete file_tree;
    delete refresh_button;
    delete input_button;
    delete output_button;
    delete delete_button;
    delete directory_button;

    delete header_layout;
    delete global_layout;
}

