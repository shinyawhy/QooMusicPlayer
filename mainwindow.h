#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPainterPath>
#include <Qtsql>
#include <QMenu>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTimer>
#include <QSettings>
#include <QDebug>
#include <QClipboard>
#include <QNetworkCookie>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextCodec>
#include <stdio.h>
#include <iostream>
#include <QtWebSockets/QWebSocket>
#include <QAuthenticator>
#include <QtConcurrent/QtConcurrent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStyledItemDelegate>
#include <QListView>
#include <QStringListModel>
#include <QScrollBar>
#include "musiclist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_close_button_clicked();

    void on_max_button_clicked();

    void on_min_button_clicked();

    void on_normal_button_clicked();

private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event) override;

    // 初始化
    void initUI();              // 初始化UI
    void initPlayer();          // 初始化播放器相关
    void initSqlite();          // 初始化sql数据库
    void initSettings();        // 初始化配置文件
    void initMusicList();       // 初始化歌单
    void initMenuAction();      // 初始化右键菜单
    void initSystemTrayIcon();  // 初始化系统托盘
    
    void isSongDownloaded(Music music);
    void downloadSong(Music music);


private:

    QMediaPlayer *player;            // 播放器
    QMediaPlaylist *playlist;        // 播放列表
    QSystemTrayIcon *mySystemTray;   // 系统托盘
    QList<MusicList *> musicList;    // 歌单
    
    const QString API_DOMAIN = "http://iwxyi.com:3000/";

};
#endif // MAINWINDOW_H
