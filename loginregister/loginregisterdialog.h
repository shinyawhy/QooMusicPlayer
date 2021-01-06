#ifndef LOGINREGISTERDIALOG_H
#define LOGINREGISTERDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QTimer>
#include <QSettings>
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

signals:
    void signalLoginFinished();

private slots:
    void on_loginButton_clicked();

    void on_registerButton_clicked();

private:
    void loginFinished();

    QSettings settings;
    Ui::LoginRegisterDialog *ui;
    QString SP;

protected:
    QString username;    // 账号
    QString password;    // 密码

    bool loginState = false;    // 登录状态
};

#endif // LOGINREGISTERDIALOG_H
