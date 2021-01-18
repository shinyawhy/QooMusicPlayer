#include "register_login.h"
#include "ui_register_login.h"

Register_Login::Register_Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Register_Login)
{
    ui->setupUi(this);
}

Register_Login::~Register_Login()
{
    delete ui;
}
