#include "loginwindow.h"


LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
{
    // display
    setFixedSize(300, 150);
    setWindowTitle("FileVault Login");
    this->move(400, 300);

    // widgets
    ps_label = new QLabel(QString("Password:"), this);
    password_edit = new QLineEdit(this);
    reset_button = new QPushButton(QString("Reset"), this);
    confirm_button = new QPushButton(QString("Confirm"), this);
    status_label = new QLabel(QString("Status: Login"), this);

    // modules
    mainwindow = new MainWindow;
    netlinker = new KernelComm;
    user = new User; string p_name = string(user->pname());
    if (netlinker->success() == false)
    {
        cout << "Can't communicate with kernel, please check if Safebox Kernel Module has been loaded." << endl;
        exit(1);
    }

    // prompt
    login_before = loginBefore();
    if (login_before)
    {
        string prompts = "Hi " + p_name + ", please input your password";
        prompt_label = new QLabel(QString::fromStdString(prompts), this);
        password_edit->setEchoMode(QLineEdit::Password);
    }
    else
    {
        string prompts = "Hi " + p_name + ", please set your password";
        prompt_label = new QLabel(QString::fromStdString(prompts), this);
    }
    password_edit->adjustSize(); prompt_label->adjustSize();

    // events
    connect(reset_button, SIGNAL(clicked()), this, SLOT(ResetPassword()));
    connect(confirm_button, SIGNAL(clicked()), this, SLOT(parsePassword()));
    connect(password_edit, SIGNAL(returnPressed()), this, SLOT(parsePassword()));

    // layout
    this->setLayouts();
}

// logic func
bool LoginWindow::loginBefore()
{
    netlinker->send_pid();

    // struct passwd *pw = get_user();
    char *password_edit = user->check_user(user->pid());
    return (password_edit == NULL) ? false:true;
}

void LoginWindow::ResetPassword()
{
    // QMessageBox::warning(this,"Warning", "Directory already exists!");
    ResetDialog();
}

void LoginWindow::ResetDialog()
{
    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Reset Password:"));
    // Value1
    QString label_old = QString("old password: ");
    // QSpinBox *spinbox1 = new QSpinBox(&dialog);
    QLineEdit *old_password = new QLineEdit(&dialog);
    form.addRow(label_old, old_password);
    // Value2
    QString label_new = QString("new password: ");
    // QSpinBox *spinbox2 = new QSpinBox(&dialog);
    QLineEdit *new_password = new QLineEdit(&dialog);
    form.addRow(label_new, new_password);
    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        QString new_pass = new_password->text();
        char *correct_pswd = user->check_user(user->pid());
        if (strcmp(correct_pswd, old_password->text().toStdString().c_str()) == 0)
        {
            user->reset_password(const_cast<char*> (new_pass.toStdString().c_str()));
        }
        else
        {
            QMessageBox::warning(this,"Warning", "The old password is wrong!");
        }
    }
}


void LoginWindow::parsePassword()
{
    QString input = password_edit->text();

    if (login_before)
    {
        // get password_edit
        netlinker->send_pid();

        char *correct_pswd = user->check_user(user->pid());

        // compare
        if (strcmp(correct_pswd, input.toStdString().c_str()) == 0)
        {
            status_label->setText(QString("Status: Login succeed"));
            this->close();
            mainwindow->show();
        }
        else
        {
            status_label->setText(QString("Status: Wrong password"));
            password_edit->setText(QString(""));
        }
    }
    else // set password
    {
        netlinker->send_pid();
        user->set_password( const_cast<char*> (input.toStdString().c_str()) );
        this->close();
        // string root_path = "/home/safebox/" + string(user->pname()) + "/";
        QString p_name = QString::fromStdString(string(user->pname()));
        QDir dir;
        dir.cd("/home/safebox/");
        dir.mkdir(p_name);
        mainwindow->show();
    }
}


void LoginWindow::setLayouts()
{
    pswd_layout = new QHBoxLayout();
    pswd_layout->addWidget(ps_label);
    pswd_layout->addWidget(password_edit);

    button_layout = new QHBoxLayout();
    button_layout->addWidget(reset_button);
    button_layout->addWidget(confirm_button);

    global_layout = new QVBoxLayout();
    global_layout->addWidget(prompt_label);
    global_layout->addLayout(pswd_layout);
    global_layout->addLayout(button_layout);
    // global_layout->addWidget(reset_button);
    // global_layout->addWidget(confirm_button);
    global_layout->addWidget(status_label);
    // global_layout->addStretch(0);

    this->setLayout(global_layout);
}


LoginWindow::~LoginWindow()
{
    delete ps_label;
    delete password_edit;
    delete reset_button;
    delete confirm_button;
    delete prompt_label;
    delete status_label;

    delete pswd_layout;
    delete button_layout;
    delete global_layout;

    delete mainwindow;
    delete netlinker;

    delete user;
}