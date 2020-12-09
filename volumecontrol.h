#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QDialog>

namespace Ui {
class volumecontrol;
}

class volumecontrol : public QDialog
{
    Q_OBJECT

public:
    explicit volumecontrol(QWidget *parent = nullptr);
    ~volumecontrol();

private:
    Ui::volumecontrol *ui;
};

#endif // VOLUMECONTROL_H
