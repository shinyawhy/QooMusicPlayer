#include "volumecontrol.h"
#include "ui_volumecontrol.h"

volumecontrol::volumecontrol(QMediaPlayer *player, QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volumecontrol), settings(settings), player(player)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    int width = this->geometry().width();
    int height = this->geometry().height();
    this->setFixedSize(width, height);
    int volume = settings.value("music/volume", 50).toInt();
    bool mute = settings.value("music/mute", false).toBool();
    if (mute)
    {
        volume = 0;
        ui->volume_Button->setIcon(QIcon(":/icon/mute"));
        ui->volume_Slider->setSliderPosition(volume);
    }
    ui->volume_Slider->setSliderPosition(volume);
    player->setVolume(volume);
}

volumecontrol::~volumecontrol()
{
    delete ui;
}

void volumecontrol::mousePressEvent(QMouseEvent *event)
{
    setAttribute(Qt::WA_NoMouseReplay);
    QDialog::mousePressEvent(event);
}


void volumecontrol::on_volume_Button_clicked()
{
    int volume = ui->volume_Slider->sliderPosition();
    if (volume == 0)
    {
        volume  = settings.value("music/volume", 50).toInt();
        if(volume == 0)
            volume = 50;
        ui->volume_Button->setIcon(QIcon(":/icon/sound"));
        ui->volume_Slider->setSliderPosition(volume);
        settings.setValue("music/mute", false);
        settings.setValue("music/volume", volume);
    }
    else
    {
        volume = 0;
        ui->volume_Button->setIcon(QIcon(":/icon/mute"));
        ui->volume_Slider->setSliderPosition(0);
        settings.setValue("music/mute", true);
    }

    player->setVolume(volume);
}



void volumecontrol::on_volume_Slider_sliderMoved(int position)
{
    player->setVolume(position);
    settings.setValue("music/volume", position);
}
