#include "loginregisterdialog.h"
#include "ui_loginregisterdialog.h"

LoginRegisterDialog::LoginRegisterDialog(QWidget *parent) : SP("http://49.235.255.205/poorrow/"),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    QDialog(parent),
    ui(new Ui::LoginRegisterDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    this->setWindowModality(Qt::WindowModal);

    this->setWindowTitle("用户登录 - 酷云音乐");

    this->setFixedSize(this->width(), this->height());

    ui->usernameWidget->setLabelText("用户名");
    ui->usernameWidget->setTipText("允许大小写字母、数字、下划线");
    ui->passwordWidget->setLabelText("密码");
    ui->passwordWidget->setTipText("任意文字");
    ui->passwordWidget->editor()->setEchoMode(QLineEdit::EchoMode::Password);

}

LoginRegisterDialog::~LoginRegisterDialog()
{
    delete ui;
}

void LoginRegisterDialog::on_loginButton_clicked()
{
    QString username = ui->usernameWidget->text();
    QString password = ui->passwordWidget->text();

    ui->passwordWidget->setMsgText("");
    ui->usernameWidget->setMsgText("");
    NetUtil* n = new NetUtil(SP + "login.php", QStringList{"username", username, "password", password});
    connect(n, &NetUtil::finished, [=](QString s){
        if (getXml(s, "STATE") == "OK")
        {
            ui->passwordWidget->showCorrect();
            ui->passwordWidget->setMsgText("登录成功");
            this->username = username;
            this->password = password;
            loginState = true;
            loginFinished();
        }
        else
        {
            ui->usernameWidget->showWrong();
            ui->usernameWidget->setMsgText("登录失败");
            ui->passwordWidget->showWrong(getXml(s, "REASON"));
        }
    });
}

void LoginRegisterDialog::on_registerButton_clicked()
{
    QString username = ui->usernameWidget->text();
    QString password = ui->passwordWidget->text();

    ui->usernameWidget->setMsgText("");
    ui->passwordWidget->setMsgText("");
    NetUtil* n = new NetUtil(SP + "register.php", QStringList{"username", username, "password", password});
    connect(n, &NetUtil::finished, [=](QString s){
        if (getXml(s, "STATE") == "OK")
        {
            ui->passwordWidget->showCorrect();
            ui->passwordWidget->setMsgText("注册成功");
        }
        else
        {
            ui->usernameWidget->showWrong();
            ui->usernameWidget->setMsgText("注册失败");
            ui->passwordWidget->showWrong(getXml(s, "REASON"));
        }
    });
}
/**
 * 登录成功以后
 */
void LoginRegisterDialog::loginFinished()
{
    // 用户信息
    settings.setValue("music/username", username);
    settings.setValue("music/password", password);
    settings.setValue("music/loginState", loginState);
    emit signalLoginFinished();

}
