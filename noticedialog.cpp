#include "noticedialog.h"
#include "ui_noticedialog.h"

NoticeDialog::NoticeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoticeDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->width(), this->height());
    this->setWindowTitle("云同步通知");

}

NoticeDialog::~NoticeDialog()
{
    delete ui;
}

void NoticeDialog::OnSyncFinished(QString s)
{
   ui->Noticelabel->setText(s);
}

