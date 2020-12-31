#ifndef LOGINREGISTERDIALOG_H
#define LOGINREGISTERDIALOG_H

#include <QDialog>

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

private:
    Ui::LoginRegisterDialog *ui;
};

#endif // LOGINREGISTERDIALOG_H
