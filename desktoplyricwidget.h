#ifndef DESKTOPLYRICWIDGET_H
#define DESKTOPLYRICWIDGET_H

#include <QWidget>
#include <windows.h>
#include <windowsx.h>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QFontMetrics>
#include <QTimer>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QInputDialog>
#include <QDir>
#include <QPushButton>
#include <QStringListModel>
#include <QDateTime>

struct LyricBean
{
    qint64 start = 0;
    qint64 end = 0;
    QString text;

    LyricBean(){}
    LyricBean(bool):start(-1),end(-1){}
};

typedef QList<LyricBean> LyricStream;

class DesktopLyricWidget : public QWidget
{
public:
    DesktopLyricWidget();
};

#endif // DESKTOPLYRICWIDGET_H
