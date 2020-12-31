#include "loginregisterdialog.h"
#include "ui_loginregisterdialog.h"

LoginRegisterDialog::LoginRegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginRegisterDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    this->setWindowModality(Qt::WindowModal);

    this->setWindowTitle("用户登录");

    this->setFixedSize(this->width(), this->height());
}

LoginRegisterDialog::~LoginRegisterDialog()
{
    delete ui;
}
