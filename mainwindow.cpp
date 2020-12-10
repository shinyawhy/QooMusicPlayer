#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    musicFileDir(QApplication::applicationDirPath() + "/musics"),
    downloadedMusicFileDir(QApplication::applicationDirPath() + "/downloaded"),
    player(new QMediaPlayer(this))
{
    ui->setupUi(this);


    // 数据库初始化
    initSqlite();

    player->setNotifyInterval(100);

    setWindowFlags(Qt::WindowSystemMenuHint | Qt::FramelessWindowHint |Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);

    FramelessHelper *pHelper = new FramelessHelper(this);
    pHelper->activateOn(this);  //激活当前窗体
    pHelper->setTitleHeight(20);  //设置窗体的标题栏高度
    pHelper->setWidgetMovable(true);  //设置窗体可移动
    pHelper->setWidgetResizable(true);  //设置窗体可缩放
    pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动
    pHelper->setRubberBandOnResize(true);  //设置橡皮筋效果-可缩放

    QHeaderView* header = ui->searchResultTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);    // 自适应列宽
    header->setMinimumSectionSize(QFontMetrics(this->font()).horizontalAdvance("一二三四五六"));
    header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//  header->setStyleSheet("QHeaderView { background-color: transparent; }");
//  ui->searchResultTable->verticalHeader()->setStyleSheet("QHeaderView { background-color: transparent; }");
    QString vScrollBarSS("QScrollBar:vertical"
                         "{"
                         "    width:7px;"
                         "    background:rgba(128,128,128,0%);"
                         "    margin:0px,0px,0px,0px;"
                         "    padding-top:0px;"
                         "    padding-bottom:0px;"
                         "}"
                         "QScrollBar::handle:vertical"
                         "{"
                         "    width:7px;"
                         "    background:rgba(128,128,128,32%);"
                         "    border-radius:3px;"
                         "    min-height:20;"
                         "}"
                         "QScrollBar::handle:vertical:hover"
                         "{"
                         "    width:7px;"
                         "    background:rgba(128,128,128,50%);"
                         "    min-height:20;"
                         "}"
                         "QScrollBar::sub-line:vertical"
                         "{"
                         "    height:9px;width:8px;"
                         "    border-image:url(:/images/a/1.png);"
                         "    subcontrol-position:top;"
                         "}"
                         "QScrollBar::add-line:vertical"
                         "{"
                         "    height:9px;width:8px;"
                         "    border-image:url(:/images/a/3.png);"
                         "    subcontrol-position:bottom;"
                         "}"
                         "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
                         "{"
                         "    background:rgba(0,0,0,0%);"
                         "    border-radius:3px;"
                         "}");

    QString hScrollBarSS("QScrollBar:horizontal"
                         "{"
                         "    height:7px;"
                         "    background:rgba(128,128,128,0%);"
                         "    margin:0px,0px,0px,0px;"
                         "    padding-left:0px;"
                         "    padding-right:0px;"
                         "}"
                         "QScrollBar::handle:horizontal"
                         "{"
                         "    height:7px;"
                         "    background:rgba(128,128,128,32%);"
                         "    border-radius:3px;"
                         "    min-width:20;"
                         "}"
                         "QScrollBar::handle:horizontal:hover"
                         "{"
                         "    height:7px;"
                         "    background:rgba(128,128,128,50%);"
                         "    min-width:20;"
                         "}"
                         "QScrollBar::sub-line:horizontal"
                         "{"
                         "    width:9px;height:8px;"
                         "    border-image:url(:/images/a/1.png);"
                         "    subcontrol-position:top;"
                         "}"
                         "QScrollBar::add-line:horizontal"
                         "{"
                         "    width:9px;height:8px;"
                         "    border-image:url(:/images/a/3.png);"
                         "    subcontrol-position:bottom;"
                         "}"
                         "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
                         "{"
                         "    background:rgba(0,0,0,0%);"
                         "    border-radius:3px;"
                         "}");
    ui->scrollArea->verticalScrollBar()->setStyleSheet(vScrollBarSS);
    ui->searchResultTable->verticalScrollBar()->setStyleSheet(vScrollBarSS);
    ui->searchResultTable->horizontalScrollBar()->setStyleSheet(hScrollBarSS);
    ui->MusicTable->verticalScrollBar()->setStyleSheet(vScrollBarSS);
    ui->MusicTable->horizontalScrollBar()->setStyleSheet(hScrollBarSS);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia)
        {
            SlotSongPlayEnd();
        }
        else if (status == QMediaPlayer::InvalidMedia)
        {
            qDebug() << "无效媒体：" << playingSong.simpleString();
            playNext();
        }
    });

    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 position){
        ui->now_duration->setText(msecondToString(position));
        slotPlayerPositionChanged();
    });

    connect(player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state){
        if (state == QMediaPlayer::PlayingState)
        {
            ui->play_button->setIcon(QIcon(":/icon/pause"));
        }
        else
        {
            ui->play_button->setIcon(QIcon(":/icon/play"));
        }
    });

    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration){
        ui->playProgressSlider->setMaximum(static_cast<int>(duration));
        if (setPlayPositionAfterLoad)
        {
            player->setPosition(setPlayPositionAfterLoad);
            setPlayPositionAfterLoad = 0;
        }
    });




    // 读取数据
    ui->stackedWidget->setCurrentIndex(settings.value("stackWidget/pageIndex").toInt());
    restoreSongList("music/order", orderSongs);
    restoreSongList("music/local", localSongs);
//    if(orderSongs.size())
//    {
//        startPlaySong(orderSongs.first());
//    }


    int volume = settings.value("music/volume", 50).toInt();
    bool mute = settings.value("music/mute", false).toBool();
    if (mute)
    {
        volume = 0;

        ui->sound_button->setIcon(QIcon(":/icon/mute"));

    }
    player->setVolume(volume);
    musicFileDir.mkpath(musicFileDir.absolutePath());

    Music currentSong = Music::fromJson(settings.value("music/currentSong").toJsonObject());
    if (currentSong.isValid())
    {
        startPlaySong(currentSong);// 还原位置

        qint64 playPosition = settings.value("music/playPosition", 0).toLongLong();
        if (playPosition)
        {
            setPlayPositionAfterLoad = playPosition;
            slotPlayerPositionChanged();
        }

        // 不自动播放
        player->play();
    }
    settings.setValue("music/playPosition", 0);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{

}



void MainWindow::initSqlite()
{

}




void MainWindow::initSystemTrayIcon()
{
    
}

QString MainWindow::songPath(const Music &music) const
{
    return musicFileDir.absoluteFilePath(snum(music.id) + ".mp3");
}

QString MainWindow::lyricPath(const Music &music) const
{
    return musicFileDir.absoluteFilePath(snum(music.id) + ".lrc");
}

QString MainWindow::coverPath(const Music &music) const
{
    return musicFileDir.absoluteFilePath(snum(music.id) + ".jpg");
}

/**
 * 歌曲是否已下载
 */
bool MainWindow::isSongDownloaded(Music music)
{
    return QFileInfo(songPath(music)).exists();
}

void MainWindow::downloadSong(Music music)
{
    if (isSongDownloaded(music))
        return;
    downloadingSong = music;
    QString url = API_DOMAIN +"/song/url?id=" + snum(music.id);
    qDebug()<< "获取歌曲信息：" << music.simpleString();
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply)
    {
        QByteArray baData = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }
        QJsonArray array = json.value("data").toArray();
        if (!array.size())
        {
            qDebug() << "未找到歌曲：" << music.simpleString();
            downloadingSong = Music();  // 清空
            downloadNext();
            return ;
        }

        json = array.first().toObject();
        QString url = JVAL_STR(url);
        int br = JVAL_INT(br); // 比率320000
        int size = JVAL_INT(size);
        QString type = JVAL_STR(type); // mp3
        QString encodeType = JVAL_STR(encodeType); // mp3
        qDebug() << "  信息：" << br << size << type << encodeType << url;
        if (size == 0)
        {
            qDebug() << "下载失败， 可能没有版权" << music.simpleString();
            if (playAfterDownloaded == music)
            {
                if (orderSongs.contains(music))
                {
                   orderSongs.removeOne(music);
                   settings.setValue("music/currentSong", "");
                   ui->playingNameLabel->clear();
                   // 设置信息
                   auto max16 = [=](QString s){
                       if (s.length() > 16)
                           s = s.left(15) + "...";
                       return s;
                   };
                   ui->playingNameLabel->setText(max16(music.name));
                   ui->playingArtistLabel->setText(max16(music.artistNames));
                   //  设置封面
                   if (QFileInfo(coverPath(music)).exists())
                   {
                       QPixmap pixmap(coverPath(music), "1");
                       if (pixmap.isNull())
                           qDebug() << "warning: 本地封面是空的" << music.simpleString() << coverPath(music);
                        // 自适应高度
                       pixmap = pixmap.scaledToHeight(ui->playingCoverLablel->height());
                       ui->playingCoverLablel->setPixmap(pixmap);
                       setCurrentCover(pixmap);
                   }
                   else
                   {
                       downloadSongCover(music);
                   }
                   player->stop();
                }
                saveSongList("music/order", orderSongs);
                setPlayListTable(orderSongs, ui->MusicTable);
                playNext();
            }
            downloadingSong = Music();
            downloadNext();
            return ;
        }

        // 下载歌曲本身
        QNetworkAccessManager manager;
        QEventLoop loop;
        QNetworkReply *reply1 = manager.get(QNetworkRequest(QUrl(url)));
        // 请求结束并完成下载后退出子事件循环
        connect(reply1, &QNetworkReply::finished, &loop, &QEventLoop::quit);

        // 开启子事件循环
        loop.exec();
        QByteArray baData1 = reply1->readAll();

        QFile file(songPath(music));
        file.open(QIODevice::WriteOnly);
        file.write(baData1);
        file.flush();
        file.close();

        emit signalSongDownLoadFinished(music);

        if (playAfterDownloaded == music)
            playLocalSong(music);

        downloadingSong = Music();
        downloadNext();
    });
    manager->get(*request);

    downloadSongLyric(music);
    downloadSongCover(music);
}

void MainWindow::downloadSongLyric(Music music)
{
    if (QFileInfo(lyricPath(music)).exists())
        return ;

    downloadingSong = music;
    QString url = API_DOMAIN + "/lyric?id=" + snum(music.id);
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray baData = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }

        QString lrc = json.value("lrc").toObject().value("lyric").toString();
        if (!lrc.isEmpty())
        {
            QFile file(lyricPath(music));
            file.open(QIODevice::WriteOnly);
            QTextStream stream(&file);
            stream << lrc;
            file.flush();
            file.close();

            qDebug() << "下载歌词完成：" << music.simpleString();
            if (playAfterDownloaded == music || playingSong == music)
            {
                setCurrentLyric(lrc);
            }

            emit signalLyricDownloadFinished(music);
        }
        else
        {
            qDebug() << "warning: 下载的歌词是空的" << music.simpleString();
        }
    });
    manager->get(*request);
}

void MainWindow::downloadSongCover(Music music)
{
    if (QFileInfo(coverPath(music)).exists())
        return ;
    downloadingSong = music;
    QString url = API_DOMAIN + "/song/detail?ids=" + snum(music.id);
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray baData = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }

        QJsonArray array = json.value("songs").toArray();
        if (!array.size())
        {
            qDebug() << "未找到歌曲：" << music.simpleString();
            downloadingSong = Music();
            downloadNext();
            return ;
        }

        json = array.first().toObject();
        QString url = json.value("al").toObject().value("picUrl").toString();
        qDebug() << "封面地址：" << url;

        // 开始下载歌曲本身
        QNetworkAccessManager manager;
        QEventLoop loop;
        QNetworkReply *reply1 = manager.get(QNetworkRequest(QUrl(url)));
        //请求结束并下载完成后，退出子事件循环
        connect(reply1, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        //开启子事件循环
        loop.exec();
        QByteArray baData1 = reply1->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(baData1);
        if (!pixmap.isNull())
        {
            QFile file(coverPath(music));
            file.open(QIODevice::WriteOnly);
            file.write(baData1);
            file.flush();
            file.close();

            emit signalCoverDownloadFinished(music);

            // 正是当前要播放的歌曲
            if (playAfterDownloaded == music || playingSong == music)
            {
                pixmap = pixmap.scaledToHeight(ui->playingCoverLablel->height());
                ui->playingCoverLablel->setPixmap(pixmap);
                setCurrentCover(pixmap);
            }
        }
        else
        {
            qDebug() << "warning: 下载的封面是空的" << music.simpleString();
        }
    });
    manager->get(*request);
}

/**
 * 立即播放
 */
void MainWindow::startPlaySong(Music music)
{
    if (isSongDownloaded(music))
    {
        playLocalSong(music);
    }
    else
    {
        playAfterDownloaded = music;
        downloadSong(music);
    }
    if (!orderSongs.contains(music))
    orderSongs.append(music);
    saveSongList("music/order", orderSongs);
    setPlayListTable(orderSongs, ui->MusicTable);
}

void MainWindow::playNext()
{
    // 播放列表结束 随机播放我的喜欢里的歌
//    if (!orderSongs.size())
//    {
//        if(!favoriteSongs.size())
//            return ;

//        int r = qrand() % favoriteSongs.size();
//        startPlaySong(favoriteSongs.at(r));
//        return ;
//    }

//    orderSongs.removeFirst();
//    saveSongList("music/order", orderSongs);
//    setPlayListTable(orderSongs, ui->MusicTable);
    qDebug()<<"111";
    int index = orderSongs.indexOf(playingSong);
        qDebug()<<index;
    // 最后一首
    if (index == orderSongs.size() - 1)
    {
        if(!favoriteSongs.size())
            return ;

        int r = qrand() % favoriteSongs.size();
        startPlaySong(favoriteSongs.at(r));
        return ;
    }
    // 歌单里不存在这首歌(放的时候被删了之类的。。
    if(index == -1)
    {
        player->stop();
        return ;
    }

//    Music music = orderSongs.first();
    Music music = orderSongs.at(index+1);
 qDebug()<<"index";
    saveSongList("music/order", orderSongs);
    setPlayListTable(orderSongs, ui->MusicTable);

    startPlaySong(music);
    emit signalOrderSongPlayed(music);

}
/**
 * 添加到歌单（队尾）
 */
void MainWindow::appendOrderSongs(SongList musics)
{
    foreach (Music music, musics)
    {
        if (orderSongs.contains(music))
            continue ;
        orderSongs.append(music);
        addDownloadSong(music);
    }

     if (isNotPlaying() && orderSongs.size())
     {
         qDebug() << "当前未播放， 开始播放列表";
         startPlaySong(orderSongs.first());
     }
     saveSongList("music/order", orderSongs);
     setPlayListTable(orderSongs, ui->MusicTable);
     downloadNext();
}

void MainWindow::removeOrderSongs(SongList musics)
{
    foreach (Music music, musics)
    {
        orderSongs.removeOne(music);
        if (playingSong == music)
        {
            settings.setValue("music/currentSong", "");
            ui->playingNameLabel->clear();
            ui->playingArtistLabel->clear();
            ui->playingCoverLablel->clear();
            player->stop();
            // 下一首歌，没有就不放
        }
    }
    playNext();
    saveSongList("music/order", orderSongs);
    setPlayListTable(orderSongs, ui->MusicTable);

}
/**
 *  下一首播放
 *  歌单队列置顶
 */
void MainWindow::appendNextSongs(SongList musics)
{
    foreach (Music music, musics)
    {
        if (orderSongs.contains(music))
            orderSongs.removeOne(music);
        // 随机播放
        if (circleMode == RandomList)
        {

        }
        int index = orderSongs.indexOf(playingSong);
        orderSongs.insert(index + 1, music);
        addDownloadSong(music);
    }

    if (isNotPlaying() && musics.size())
    {
        qDebug() << "当前未播放， 开始播放本首歌";
        startPlaySong(orderSongs.first());
    }
    saveSongList("music/order", orderSongs);
    setPlayListTable(orderSongs, ui->MusicTable);
    downloadNext();
}

void MainWindow::setCurrentCover(const QPixmap &pixmap)
{
    currentCover = pixmap;
}

void MainWindow::setCurrentLyric(QString lyric)
{

}

void MainWindow::addFavorite(SongList musics)
{
    foreach (Music music, musics)
    {
        if (favoriteSongs.contains(music))
        {
           qDebug() << "歌曲已存在：" << music.simpleString();
           continue;
        }
        favoriteSongs.append(music);
        qDebug() <<"添加收藏：" << music.simpleString();
    }
    saveSongList("music/favorite", favoriteSongs);
}

void MainWindow::removeFavorite(SongList musics)
{
    foreach (Music music, musics)
    {
        if (favoriteSongs.removeOne(music))
        {
            qDebug() << "取消收藏：" << music.simpleString();
        }
        saveSongList("music/favorite", favoriteSongs);
    }
}

void MainWindow::saveSongList(QString key, const SongList &songs)
{
    QJsonArray array;
    foreach(Music music, songs)
        array.append(music.toJson());
    settings.setValue(key,array);

}

void MainWindow::restoreSongList(QString key, SongList &songs)
{
    QJsonArray array = settings.value(key).toJsonArray();
    foreach(QJsonValue val, array)
        songs.append(Music::fromJson(val.toObject()));
}

void MainWindow::searchMusic(QString key)
{
    if (key.trimmed().isEmpty())
        return;    
    QString url = "http://iwxyi.com:3000/search?keywords=" + key.toUtf8().toPercentEncoding();
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray data = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return;
        }

        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return;
        }

        QJsonArray musics = json.value("result").toObject().value("songs").toArray();
        // 清除上次搜索结果
        searchResultSongs.clear();
        foreach (QJsonValue val, musics)
        {
            searchResultSongs << Music::fromJson(val.toObject());
        }

        setSearchResultTable(searchResultSongs);


    });
    manager->get(*request);

}
/**
 * 搜索数据结果到Table
 */
void MainWindow::setSearchResultTable(SongList songs)
{
    // 建立表
    QTableWidget* table = ui->searchResultTable;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 清空旧数据
    table->clear();
    searchResultPlayLists.clear();

    // 设置Table标题
    enum{
        titleCol,
        artistCol,
        albumCol,
        durationCol
    };
    table->setColumnCount(4);
    QStringList headers{"标题", "作者", "专辑", "时长"};
    table->setHorizontalHeaderLabels(headers);

    // 设置列长度
    QFontMetrics fm(font());
    int fw = fm.horizontalAdvance("一二三四五六七八九十十一十二十三十四十五");
    auto createItem = [=](QString s){
        QTableWidgetItem *item = new QTableWidgetItem();
        if (s.length() > 16 && fm.horizontalAdvance(s) > fw)
        {
            item->setToolTip(s);
            s = s.left(15) + "...";
        }
        item->setText(s);
        return item;
    };

    table->setRowCount(songs.size());
    for (int row = 0; row < songs.size(); row++)
    {
       Music song = songs.at(row);
       table->setItem(row, titleCol, createItem(song.name));
       table->setItem(row, artistCol, createItem(song.artistNames));
       table->setItem(row, albumCol, createItem(song.album.name));
       table->setItem(row, durationCol, createItem(msecondToString(song.duration)));
    }

    QTimer::singleShot(0, [=]{
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    });
}

void MainWindow::setPlayListTable(SongList songs, QTableWidget *table)
{
//    QTableWidget* table = ui->MusicTable;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    table->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    table->clear();

    // 设置Table标题
    enum{
        titleCol,
        artistCol,
        albumCol,
        durationCol
    };
    table->setColumnCount(4);
    QStringList headers{"标题", "作者", "专辑", "时长"};
    table->setHorizontalHeaderLabels(headers);
    for (int i = 0; i < 4; i++)
    {
        table->horizontalHeaderItem(i)->setSizeHint(QSize(ui->Musicpage->width() / 4, 32));
    }
    // 设置列长度
    QFontMetrics fm(font());
    int fw = fm.horizontalAdvance("一二三四五六七八九十十一十二十三十四十五");
    auto createItem = [=](QString s){
        QTableWidgetItem *item = new QTableWidgetItem();
        if (s.length() > 16 && fm.horizontalAdvance(s) > fw)
        {
            item->setToolTip(s);
            s = s.left(15) + "...";
        }
        item->setText(s);
        return item;
    };

    table->setRowCount(songs.size());
    for (int row = 0; row < songs.size(); row++)
    {
       Music song = songs.at(row);
       table->setItem(row, titleCol, createItem(song.name));
       table->setItem(row, artistCol, createItem(song.artistNames));
       table->setItem(row, albumCol, createItem(song.album.name));
       table->setItem(row, durationCol, createItem(msecondToString(song.duration)));
    }

    QTimer::singleShot(0, [=]{
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    });

}

/**
 * 立即开始播放音乐
 */
void MainWindow::playLocalSong(Music music)
{
    qDebug() << "开始播放" << music.simpleString();
    if (!isSongDownloaded(music))
    {
        qDebug() << "error:未下载歌曲：" << music.simpleString() << "开始下载";
        playAfterDownloaded = music;
        downloadSong(music);
        return ;
    }

    // 设置信息
    auto max16 = [=](QString s){
        if (s.length() > 16)
            s = s.left(15) + "...";
        return s;
    };
    ui->playingNameLabel->setText(max16(music.name));
    ui->playingArtistLabel->setText(max16(music.artistNames));
    ui->all_duration->setText(msecondToString(music.duration));
    //  设置封面
    if (QFileInfo(coverPath(music)).exists())
    {
        QPixmap pixmap(coverPath(music), "1");
        if (pixmap.isNull())
            qDebug() << "warning: 本地封面是空的" << music.simpleString() << coverPath(music);
         // 自适应高度
        pixmap = pixmap.scaledToHeight(ui->playingCoverLablel->height());
        ui->playingCoverLablel->setPixmap(pixmap);
        setCurrentCover(pixmap);
    }
    else
    {
        downloadSongCover(music);
    }

    // 开始播放
    playingSong = music;
    player->setMedia(QUrl::fromLocalFile(songPath(music)));
    player->setPosition(0);
    player->play();
    emit signalSongPlayStarted(music);
    setWindowTitle(music.name);

    // 添加到本地歌曲
    if (localSongs.contains(music))
    {
        qDebug() << "本地歌曲中已存在：" << music.simpleString();
        return ;
    }
    else
    {
        localSongs.append(music);
        saveSongList("music/local", localSongs);
    }

    // 保存当前歌曲
    settings.setValue("music/currentSong", music.toJson());

}

/**
 * 放入下载队列，准备下载（并不立即下载）
 */
void MainWindow::addDownloadSong(Music music)
{
    if (isSongDownloaded(music) || toDownLoadSongs.contains(music) || downloadingSong == music)
       return ;
    toDownLoadSongs.append(music);
}

/**
 * 放入下载队列、或一首歌下载结束后，下载下一个
 */
void MainWindow::downloadNext()
{
    // 正在下载的歌已经下载了/即将下载的队列是空的 bool 0 false 1 true
    if (downloadingSong.isValid() || !toDownLoadSongs.size())
        return ;
    Music music = toDownLoadSongs.takeFirst();
    if (!music.isValid())
        return downloadNext();
    downloadSong(music);
}

QString MainWindow::msecondToString(qint64 msecond)
{
    return QString("%1:%2").arg(msecond / 1000 / 60, 2, 10, QLatin1Char('0'))
            .arg(msecond / 1000 % 60, 2, 10, QLatin1Char('0'));
}

void MainWindow::activeSong(Music music)
{
    startPlaySong(music);
    appendOrderSongs(SongList{music});
}

bool MainWindow::isNotPlaying() const
{
    return player->state() != QMediaPlayer::PlayingState
            && (!playingSong.isValid() || player->position() == 0);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isMousePressed == true)
    {
        QPoint movePot = event->globalPos() - mousePosition;
        move(movePot);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mousePosition = event->pos();
    // 只对标题栏范围内的鼠标事件进行处理
    if(mousePosition.y() <= (ui->widget_3->pos().y() + ui->widget_3->geometry().bottom()))
    {
        isMousePressed = true;
    }
    return QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
    return QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *)
{
    if(mousePosition.y() <= (ui->widget_3->pos().y() + ui->widget_3->geometry().bottom()))
    {
        if(windowState() == Qt::WindowMaximized)
        {
        on_normal_button_clicked();
        }
        else
        on_max_button_clicked();
    }
}
/**
 * 重写窗体缩放， 仅在Windows环境下有效
 */
//bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//    Q_UNUSED(eventType)

//    MSG *param = static_cast<MSG *>(message);

//    switch (param->message) {
//    case WM_NCHITTEST:
//    {
//       int nX = GET_X_LPARAM(param->lParam) - this->geometry().x();
//       int nY = GET_Y_LPARAM(param->lParam) - this->geometry().y();
//       // 鼠标位于子控件上， 不进行处理
//       if ((childAt(nX, nY) != nullptr && childAt(nX, nY) != ui->centralwidget)
//               || nY <= (ui->widget_3->pos().y() + ui->widget_3->geometry().bottom()))
//       {
//           return QWidget::nativeEvent(eventType, message, result);
//       }
//       *result = HTCAPTION;

//       // 鼠标位于窗体边框， 进行缩放
//       if ((nX > 0) && (nX < 5))
//           *result = HTLEFT;

//       if ((nX > this->width() - 5) && (nX < this->width()))
//           *result = HTRIGHT;

//       if ((nY > 0) && (nY < 5))
//           *result = HTTOP;

//       if ((nY > this->height() - 5) && (nY < this->height()))
//           *result = HTBOTTOM;

//       if ((nX > 0) && (nX < 5) && (nY > 0)
//               && (nY < 5))
//           *result = HTTOPLEFT;

//       if ((nX > this->width() - 5) && (nX < this->width())
//               && (nY > 0) && (nY < 5))
//           *result = HTTOPRIGHT;

//       if ((nX > 0) && (nX < 5)
//               && (nY > this->height() - 5) && (nY < this->height()))
//           *result = HTBOTTOMLEFT;

//       if ((nX > this->width() - 5) && (nX < this->width())
//               && (nY > this->height() - 5) && (nY < this->height()))
//           *result = HTBOTTOMRIGHT;

//       return true;
//    }
//    }
//    return QWidget::nativeEvent(eventType, message, result);
//}

void MainWindow::on_close_button_clicked()
{
    close();
}

void MainWindow::on_max_button_clicked()
{
    setWindowState(Qt::WindowMaximized);
}

void MainWindow::on_min_button_clicked()
{
    setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_normal_button_clicked()
{
    showNormal();
}

void MainWindow::on_search_button_clicked()
{
    on_search_edit_returnPressed();
}

void MainWindow::on_search_edit_returnPressed()
{
    ui->searchResultTable->clear();
    QString text = ui->search_edit->text();
    searchMusic(text);
    ui->stackedWidget->setCurrentWidget(ui->searchResultPage);
}

void MainWindow::on_searchResultTable_itemActivated(QTableWidgetItem *item)
{
    if (searchResultSongs.size())
    {
        Music music = searchResultSongs.at(item->row());
    }
    else if (searchResultPlayLists.size())
    {

    }
}

void MainWindow::on_searchResultTable_customContextMenuRequested(const QPoint &)
{
    auto items= ui->searchResultTable->selectedItems();

    // 是歌曲搜索结果
    if (searchResultSongs.size())
    {
        QList<Music> musics;
        foreach (auto item, items)
        {
            int row = ui->searchResultTable->row(item);
            int col = ui->searchResultTable->column(item);
            if (col != 0)
                continue;
            musics.append(searchResultSongs.at(row));
        }
        int row = ui->searchResultTable->currentRow();
        Music currentsong;
        if (row > -1)
                currentsong = searchResultSongs.at(row);
        // 创建菜单对象
        QMenu* menu = new QMenu(this);

        QAction *playNow = new QAction("立即播放",this);
        QAction *playNext = new QAction("下一首播放", this);
        QAction *addToPlayList = new QAction("添加到播放列表", this);
        QAction *Favorite = new QAction("我的喜欢", this);
        if (favoriteSongs.contains(currentsong))
            Favorite->setText("从我的喜欢中移除");
        else
            Favorite->setText("添加到我的喜欢");

        // QAction对象添加到菜单上
        menu->addAction(playNow);
        menu->addAction(playNext);
        menu->addAction(addToPlayList);
        menu->addAction(Favorite);

        // 链接鼠标右键点击信号
        connect(playNow, &QAction::triggered, [=]{
            startPlaySong(currentsong);
        });

        connect(playNext, &QAction::triggered, [=]{
           appendNextSongs(musics);
        });

        connect(addToPlayList, &QAction::triggered, [=]{
            appendOrderSongs(musics);
        });

        connect(Favorite, &QAction::triggered, [=]{
            if(!favoriteSongs.contains(currentsong))
                addFavorite(musics);
            else
                removeFavorite(musics);

        });

        // 显示菜单
        menu->exec(cursor().pos());

        // 释放内存
        QList<QAction*> list = menu->actions();
        foreach(QAction* action, list)
            delete action;
        delete menu;
    }
}

void MainWindow::on_play_list_button_clicked()
{
    setPlayListTable(orderSongs, ui->MusicTable);
    ui->stackedWidget->setCurrentWidget(ui->Musicpage);
}

void MainWindow::on_stackedWidget_currentChanged(int index)
{
    settings.setValue("stackWidget/pageIndex", index);
}

void MainWindow::on_MusicTable_customContextMenuRequested(const QPoint &)
{
    auto items = ui->MusicTable->selectedItems();

    QList<Music> musics;
    foreach(auto item, items)
    {
        int row = ui->MusicTable->row(item);
        int col = ui->MusicTable->column(item);
        if (col != 0)
            continue;
        musics.append(orderSongs.at(row));
    }
    int row = ui->MusicTable->currentRow();
    Music currentsong;
    if (row > -1)
        currentsong = orderSongs.at(row);

    QMenu* menu = new QMenu(this);

    QAction *playNow = new QAction("立即播放", this);
    QAction *playNext = new QAction("下一首播放", this);
    QAction *removeToPlayList = new QAction("从播放列表中移除", this);
    QAction *Favorite = new QAction("我的喜欢", this);
    if (favoriteSongs.contains(currentsong))
        Favorite->setText("从我的喜欢中移除");
    else
        Favorite->setText("添加到我的喜欢");

    menu->addAction(playNow);
    menu->addAction(playNext);
    menu->addAction(removeToPlayList);
    menu->addAction(Favorite);

    connect(playNow, &QAction::triggered, [=]{
       startPlaySong(currentsong);
    });

    connect(playNext, &QAction::triggered, [=]{
       appendNextSongs(musics);
    });

    connect(removeToPlayList, &QAction::triggered, [=]{
        removeOrderSongs(musics);
    });

    connect(Favorite, &QAction::triggered, [=]{
        if (!favoriteSongs.contains(currentsong))
            addFavorite(musics);
        else
            removeFavorite(musics);
    });

    // 显示菜单
    menu->exec(cursor().pos());

    // 释放内存
    QList<QAction*> list = menu->actions();
    foreach(QAction* action, list)
        delete action;
    delete menu;
}

void MainWindow::on_local_music_button_clicked()
{
    setPlayListTable(localSongs, ui->localMusicTable);
    ui->stackedWidget->setCurrentWidget(ui->localMusicpage);
}

void MainWindow::on_localMusicTable_customContextMenuRequested(const QPoint &)
{
    auto items = ui->localMusicTable->selectedItems();

    QList<Music> musics;
    foreach (auto item, items)
    {
        int row = ui->localMusicTable->row(item);
        int col = ui->localMusicTable->column(item);
        if (col != 0)
            continue;
        musics.append(localSongs.at(row));
    }
    int row = ui->localMusicTable->currentRow();
    Music currentsong;
    if (row > -1)
        currentsong = localSongs.at(row);

    QMenu* menu = new QMenu(this);

    QAction *playNow = new QAction("立即播放", this);
    QAction *playNext = new QAction("下一首播放", this);
    QAction *addToPlayList = new QAction("添加到播放列表", this);
    QAction *Favorite = new QAction("我的喜欢", this);
    if (favoriteSongs.contains(currentsong))
        Favorite->setText("从我的喜欢中移除");
    else
        Favorite->setText("添加到我的喜欢");

    menu->addAction(playNow);
    menu->addAction(playNext);
    menu->addAction(addToPlayList);
    menu->addAction(Favorite);

    // 链接鼠标右键点击信号
    connect(playNow, &QAction::triggered, [=]{
        startPlaySong(currentsong);
    });

    connect(playNext, &QAction::triggered, [=]{
       appendNextSongs(musics);
    });

    connect(addToPlayList, &QAction::triggered, [=]{
        appendOrderSongs(musics);
    });

    connect(Favorite, &QAction::triggered, [=]{
        if(!favoriteSongs.contains(currentsong))
            addFavorite(musics);
        else
            removeFavorite(musics);

    });

    // 显示菜单
    menu->exec(cursor().pos());

    // 释放内存
    QList<QAction*> list = menu->actions();
    foreach(QAction* action, list)
        delete action;
    delete menu;

}


void MainWindow::on_searchResultTable_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = ui->searchResultTable->row(item);
    Music currentsong;
    if (row > -1 )
        currentsong = searchResultSongs.at(row);
    if (orderSongs.contains(currentsong))
    {
        orderSongs.removeOne(currentsong);
        setPlayListTable(orderSongs, ui->MusicTable);
    }
    else
        orderSongs.insert(0, currentsong);
    startPlaySong(currentsong);
}

void MainWindow::SlotSongPlayEnd()
{
    emit signalSongPlayFinished(playingSong);

    //清除播放
//    playingSong = Music();
    settings.setValue("music/currentSong", "");
    ui->playingNameLabel->clear();
    ui->playingArtistLabel->clear();
    ui->playingCoverLablel->clear();

    playNext();
}

/**
 * 暂停、播放
 */
void MainWindow::on_play_button_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
//        ui->play_button->setIcon(QIcon(":/icon/play"));
    }
    else
    {
        if (!playingSong.isValid())
        {
            playNext();
            return ;
        }
        player->play();
//        ui->play_button->setIcon(QIcon(":icon/pause"));
    }
}

void MainWindow::on_sound_button_clicked()
{
    volumecontrol *vc = new volumecontrol(player, settings, this);
    int height = vc->height();
    int width = vc->width();
    int x = ui->sound_button->pos().x();
    int y = ui->sound_button->pos().y();
    int pos_x = x  + (ui->sound_button->width() / 2) - (width/ 2);
    int pos_y = y - (height + (ui->sound_button->height() / 2));

    vc->setGeometry(QRect(mapToGlobal(QPoint(pos_x, pos_y)), QSize(width, height)));

    vc->exec();
    vc->deleteLater();
}

void MainWindow::slotPlayerPositionChanged()
{
   qint64 position = player->position();
   ui->playProgressSlider->setSliderPosition(static_cast<int>(position));
   update();
}

void MainWindow::on_playProgressSlider_sliderMoved(int position)
{
    player->setPosition(position);
}

