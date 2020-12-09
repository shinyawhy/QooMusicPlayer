#include "volumecontrol.h"
#include "ui_volumecontrol.h"

volumecontrol::volumecontrol(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volumecontrol)
{
    ui->setupUi(this);
}

volumecontrol::~volumecontrol()
{
    delete ui;
}
