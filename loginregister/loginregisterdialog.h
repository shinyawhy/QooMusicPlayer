#ifndef LOGINREGISTERDIALOG_H
#define LOGINREGISTERDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "netutil.h"
#include "stringutil.h"

namespace Ui {
class LoginRegisterDialog;
}

class LoginRegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginRegisterDialog(QWidget *parent = nullptr);
    ~LoginRegisterDialog();

private slots:
    void on_loginButton_clicked();

    void on_registerButton_clicked();

private:
    Ui::LoginRegisterDialog *ui;
    QString SP;
};

#endif // LOGINREGISTERDIALOG_H
