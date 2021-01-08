#ifndef NOTICEDIALOG_H
#define NOTICEDIALOG_H

#include <QDialog>

namespace Ui {
class NoticeDialog;
}

class NoticeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoticeDialog(QWidget *parent = nullptr);
    ~NoticeDialog();
    void OnSyncFinished(QString s);

private:
    Ui::NoticeDialog *ui;


};

#endif // NOTICEDIALOG_H
