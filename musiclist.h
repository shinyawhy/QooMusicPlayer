#ifndef MUSICLIST_H
#define MUSICLIST_H
#include <QList>
#include <QMediaPlaylist>
#include <QListWidget>
#include "musicbeans.h"

class MusicListWidget;

// 歌单列表
class MusicList{
    friend class MainWindow;

    QString name;           // 歌单名
    QList<Music *> music;   // 所含歌曲

    bool sql_flag = true;   // 数据库交互开关

public:
    MusicList(){}

    MusicList(const QList<QUrl>& urls, QString iname = "");

    // 歌单名操作
    void setName(const QString& iname); // 设置歌单名
    QString getName();                  // 获取歌单名

    // 歌曲操作
    void setSQL(bool on);
    void addMusic(const QList<QUrl>& urls); // 从url添加个歌曲
    void addMusic(const Music& iMusic);     // 添加一首歌曲
    Music getMusic(int pos);                // 获取指定位置的歌曲

    void addToPlayList(QMediaPlaylist *playlist);       // 歌单加入播放列表
    void addToListWidget(MusicListWidget *listWidget);  // 显示歌单
    void removeMusic(int pos);                          // 删除指定位置歌曲
    void showInExplorer(int pos);                       // 在文件夹中打开
    void showDetail(int pos);                           // 显示歌曲详细信息
    void remove_all();                                  // 数据库中删除歌单全部歌曲
    void insert_all();                                  // 歌单写入数据库
    void read_SQL();                                    // 导入数据库中歌单
    void clear();                                       // 清空歌单



};

#endif // MUSICLISTWIDGET_H
