#ifndef REGISTER_LOGIN_H
#define REGISTER_LOGIN_H

#include <QMainWindow>

namespace Ui {
class Register_Login;
}

class Register_Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Register_Login(QWidget *parent = nullptr);
    ~Register_Login();

private:
    Ui::Register_Login *ui;
};

#endif // REGISTER_LOGIN_H
