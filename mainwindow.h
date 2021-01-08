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
#include <QSlider>
#include <QList>
#include <QMediaPlaylist>
#include <QListWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QDesktopServices>
#include "tipbox.h"
#include "musicbeans.h"
//#include "musiclist.h"
#include "volumecontrol.h"
#include "frameless_helper.h"
#include "imageutil.h"
#include "lyricstreamwidget.h"
#include "desktoplyricwidget.h"
#include "mymenu.h"
#include "loginregisterdialog.h"
#include "netutil.h"
#include "stringutil.h"
#include "noticedialog.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <Windowsx.h>
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE
    extern Q_WIDGETS_EXPORT void qt_blurImage( QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0 );
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int lyricScroll READ getLyricScroll WRITE setLyricScroll)
    Q_PROPERTY(int disappearBgProg READ getDisappearBgProg WRITE setDisappearBgProg)
    Q_PROPERTY(int appearBgProg READ getAppearBgProg WRITE setAppearBgProg)
    Q_PROPERTY(double paletteProg READ getPaletteBgProg WRITE setPaletteBgProg)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum PlayCirecleMode
    {
      OrderList,    // 顺序
      SingleList,   // 单曲
      RandomList,   // 随机

    };

    struct BFSColor
    {
        int v[12] = {0};

        BFSColor()
        {}

        BFSColor(QList<QColor> cs)
        {
           Q_ASSERT(cs.size() == 4);
            for (int i = 0; i < 4; i++)
            {
                QColor c = cs[i];
                v[i * 3 + 0] = c.red();
                v[i * 3 + 1] = c.green();
                v[i * 3 + 2] = c.blue();
            }
        }

        BFSColor operator-(const BFSColor& ano)
        {
            BFSColor bfs;
            for (int i = 0; i < 12; i++)
                bfs.v[i] = this->v[i] - ano.v[i];
            return bfs;
        }

        BFSColor operator+(const BFSColor& ano)
        {
            BFSColor bfs;
            for (int i = 0; i < 12; i++)
                bfs.v[i] = this->v[i] + ano.v[i];
            return bfs;
        }

        BFSColor operator*(const double& prop)
        {
            BFSColor bfs;
            for (int i = 0; i < 12; i++)
                bfs.v[i] = this->v[i] * prop;
            return bfs;
        }

        static BFSColor fromPalette(QPalette pa)
        {
            QColor bg = pa.color(QPalette::Window);
            QColor fg = pa.color(QPalette::Text);
            QColor sbg = pa.color(QPalette::Highlight);
            QColor sfg = pa.color(QPalette::HighlightedText);
            return BFSColor(QList<QColor>{bg, fg, sbg, sfg});
        }

        void toColors(QColor* bg, QColor* fg, QColor* sbg, QColor* sfg)
        {
            *bg = QColor(v[0], v[1], v[2]);
            *fg = QColor(v[3], v[4], v[5]);
            *sbg = QColor(v[6], v[7], v[8]);
            *sfg = QColor(v[9], v[10], v[11]);
        }
    };

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

    void on_local_music_button_clicked();

    void on_localMusicTable_customContextMenuRequested(const QPoint &pos);

    void on_searchResultTable_itemDoubleClicked(QTableWidgetItem *item);

    void SlotSongPlayEnd();

    void on_play_button_clicked();

    void on_sound_button_clicked();

    void slotPlayerPositionChanged();

    void on_playProgressSlider_sliderMoved(int position);

    void on_mode_button_clicked();

    void on_forward_button_clicked();

    void slotExpandPlayingButtonClicked();

    void on_back_button_clicked();

    void on_MusicTable_itemDoubleClicked(QTableWidgetItem *item);

    void on_lyric_button_clicked();

    void adjustCurrentLyricTime(QString lyric);

    void on_my_favorite_button_clicked();

    void on_FavoriteMusicTable_customContextMenuRequested(const QPoint &pos);

    void on_FavoriteMusicTable_itemDoubleClicked(QTableWidgetItem *item);

    void on_my_song_list_button_clicked();

    void on_add_list_Button_clicked();

    void on_MusiclistWidget_customContextMenuRequested(const QPoint &pos);

    void Play_Playlist(QList<QListWidgetItem *> items);

    void on_MusiclistWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_MusiclistWidget_itemClicked(QListWidgetItem *item);

    void on_PlayListTable_itemDoubleClicked(QTableWidgetItem *item);

    void on_PlayListTable_customContextMenuRequested(const QPoint &pos);

    void on_logo_button_clicked();
    
    void on_LogoutButton_clicked();

    void on_SyncButton_clicked();

    void ShowPlaylistTabMenu(QListView *QLV);

private:
    Ui::MainWindow *ui;

    // 初始化
    void initSqlite();          // 初始化sql数据库

    void systemTrayIcon_actived(QSystemTrayIcon::ActivationReason reason);
    
    QString songPath(const Music &music) const;
    QString lyricPath(const Music &music) const;
    QString coverPath(const Music &music) const;
    bool isSongDownloaded(Music music);

    void searchMusic(QString key);                      // 搜索音乐
    void setSearchResultTable(SongList songs);          // 搜索结果数据到table
    void setSearchResultTable(PlayListList playLists);
    void setPlayListTable(QList<Music> songs, QTableWidget* table);
    void setPLAYLISTTable(PlayListList playlist, QListWidget* list);
    void setPlaylistTab(PlayListList pl, QTabWidget* tab);

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
    void appendMusicToPlayList(SongList musics, int row);
    void ClearPlayList();

    void setCurrentCover(const QPixmap& pixmap);
    void setCurrentLyric(QString lyric);
    void adjustExpandPlayingButton();
    void setBlurBackground(const QPixmap& bg);
    void setThemeColor(const QPixmap& cover);
    void connectDesktopLyricSignals();

    void addFavorite(SongList musics);
    void removeFavorite(SongList musics);

    // 保存配置
    void saveSongList(QString key, const SongList &songs);
    void savePlayList(QString key, const PlayListList &playlistlist);
    // 读取配置
    void restoreSongList(QString key, SongList &songs);
    void restorePlayList(QString key, PlayListList &playlistlist);

    // QJSON QString互相转化
    QJsonArray QstringToJson(QString jsonString);
    QString JsonToQstring(QJsonArray jsonArray);


private:
    void setAppearBgProg(int x);
    void setDisappearBgProg(int x);
    void setPaletteBgProg(double x);
    void setLyricScroll(int x);
    int getLyricScroll() const;
    int getAppearBgProg() const;
    int getDisappearBgProg() const;
    double getPaletteBgProg() const;



private:

    QMediaPlayer *player;            // 播放器
    QMediaPlaylist *playlist;        // 播放列表
    QSystemTrayIcon *mySystemTray;   // 系统托盘

    PlayCirecleMode circleMode = OrderList;
    QAction *action_systemTray_playmode;
    QAction *music_info;

    SongList searchResultSongs;
    PlayListList searchResultPlayLists;
    Music downloadingSong;
    Music playAfterDownloaded;
    Music playingSong;

    SongList orderSongs;             // 播放列表
    SongList favoriteSongs;          // 我的喜欢
    SongList localSongs;             // 本地歌曲
    SongList toDownLoadSongs;        // 即将下载

    PlayListList PLAYLIST;           // 歌单

    TipBox* tip_box;

    bool isSongDownFailed = false;
    qint64 setPlayPositionAfterLoad = 0; // 加载后跳转到时间

    QString msecondToString(qint64 msecond);
    void activeSong(Music music);
    bool isNotPlaying() const;
    int onMusicAppendRandom = 0;
    int lyricScroll;
    
    QSettings settings;
    QDir musicFileDir;   // 用于程序识别
    QDir downloadedMusicFileDir; // 已下载的歌曲资源
    const QString API_DOMAIN = "http://iwxyi.com:3000/";

    QPushButton* expandPlayingButton;
    DesktopLyricWidget* desktopLyric;

    bool blurBg = true;
    int blurAlpha = 32;
    int currentBgAlpha = 255;
    int prevBgAlpha = 0;


    QPixmap currentCover;
    QPixmap currentBlurBg;
    QPixmap prevBlurBg;

    bool themeColor = false;
    BFSColor prevPa;
    BFSColor currentPa;
    double paletteAlpha;

    QString SP;
    bool loginState = false;    // 登录状态

protected:
    // 重新实现拖动操作
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    void showEvent(QShowEvent*) override;
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void paintEvent(QPaintEvent* e) override;


//    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    QPoint mousePosition;
    bool isMousePressed = false;

    const static int pos_min_x = 0;
    const static int pos_min_y = 0;


signals:
    void signalOrderSongPlayed(Music music);
    void signalSongDownLoadFinished(Music music);
    void signalLyricDownloadFinished(Music music);
    void signalCoverDownloadFinished(Music music);

    void signalSongPlayStarted(Music music);
    void signalSongPlayFinished(Music music);

    void signalLogout();

};
#endif // MAINWINDOW_H
