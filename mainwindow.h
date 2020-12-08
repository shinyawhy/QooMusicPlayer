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
#include <QHeaderView>
#include <QTableWidget>
#include "musiclist.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <Windowsx.h>
#endif

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

    void on_search_button_clicked();

    void on_search_edit_returnPressed();

    void on_searchResultTable_itemActivated(QTableWidgetItem *item);

    void on_searchResultTable_customContextMenuRequested(const QPoint &pos);

    void on_play_list_button_clicked();

    void on_stackedWidget_currentChanged(int index);

    void on_MusicTable_customContextMenuRequested(const QPoint &pos);

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
    
    QString songPath(const Music &music) const;
    QString lyricPath(const Music &music) const;
    QString coverPath(const Music &music) const;
    bool isSongDownloaded(Music music);

    void searchMusic(QString key);                      // 搜索音乐
    void setSearchResultTable(SongList songs);          // 搜索结果数据到table
    void setSearchResultTable(PlayListList playLists);
    void setPlayListTable(SongList songs);

    // 下载音乐
    void playLocalSong(Music music);
    void addDownloadSong(Music music);
    void downloadNext();
    void downloadSong(Music music);
    void downloadSongLyric(Music music);
    void downloadSongCover(Music music);

    // 播放音乐
    void startPlaySong(Music music);
    void playNext();
    void appendOrderSongs(SongList musics);
    void removeOrderSongs(SongList musics);
    void appendNextSongs(SongList musics);

    void setCurrentCover(const QPixmap& pixmap);
    void setCurrentLyric(QString lyric);

    void addFavorite(SongList musics);
    void removeFavorite(SongList musics);

    // 保存配置
    void saveSongList(QString key, const SongList &songs);
    // 读取配置
    void restoreSongList(QString key, SongList &songs);



private:

    QMediaPlayer *player;            // 播放器
    QMediaPlaylist *playlist;        // 播放列表
    QSystemTrayIcon *mySystemTray;   // 系统托盘
    QList<MusicList *> musicList;    // 歌单

    SongList searchResultSongs;
    PlayListList searchResultPlayLists;
    Music downloadingSong;
    Music playAfterDownloaded;
    Music playingSong;

    SongList orderSongs;             // 播放列表
    SongList favoriteSongs;          // 我的喜欢
    SongList localSongs;             // 本地歌曲
    SongList toDownLoadSongs;        // 即将下载

    QPixmap currentCover;


    QString msecondToString(qint64 msecond);
    void activeSong(Music music);
    bool isNotPlaying() const;
    
    QSettings settings;
    QDir musicFileDir;
    const QString API_DOMAIN = "http://iwxyi.com:3000/";

protected:
    // 重新实现拖动操作
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    QPoint mousePosition;
    bool isMousePressed;

    const static int pos_min_x = 0;
    const static int pos_min_y = 0;


signals:
    void signalOrderSongPlayed(Music music);
    void signalSongDownLoadFinished(Music music);
    void signalLyricDownloadFinished(Music music);
    void signalCoverDownloadFinished(Music music);

    void signalSongPlayStarted(Music music);
};
#endif // MAINWINDOW_H
