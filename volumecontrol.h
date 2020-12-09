#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QDialog>
#include <QSettings>
#include <QMediaPlayer>
namespace Ui {
class volumecontrol;
}

class volumecontrol : public QDialog
{
    Q_OBJECT

public:
    explicit volumecontrol(QMediaPlayer* player, QSettings &settings, QWidget *parent = nullptr);
    ~volumecontrol();

private:
    Ui::volumecontrol *ui;

    QSettings &settings;
    QMediaPlayer *player;

protected:
    void mousePressEvent(QMouseEvent *event) override;
private slots:

    void on_volume_Button_clicked();

    void on_volume_Slider_sliderMoved(int position);
};

#endif // VOLUMECONTROL_H
