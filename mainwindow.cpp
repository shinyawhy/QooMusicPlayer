#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    musicFileDir(QApplication::applicationDirPath() + "/musics"),
    downloadedMusicFileDir(QApplication::applicationDirPath() + "/downloaded"),
    player(new QMediaPlayer(this)),
    expandPlayingButton(new QPushButton(this)),
    mySystemTray(new QSystemTrayIcon(this)),
    desktopLyric(new DesktopLyricWidget()),
    SP("http://49.235.255.205/poorrow/"),tip_box(new TipBox(this)),
    action_systemTray_playmode(new QAction(this)), music_info(new QAction(this))
{
    ui->setupUi(this);

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
    ui->SyncButton->setStyleSheet("text-align:left");

//    tip_box = new TipBox(this);
    connect(tip_box, &TipBox::signalCardClicked, [=](NotificationEntry* n){
        qDebug() << n->toString();
    });
    connect(tip_box, &TipBox::signalBtnClicked, [=](NotificationEntry* n){
        qDebug() << n->toString();
    });

    QScreen* screen = QApplication::screenAt(QCursor::pos());
    QRect rect = screen->availableGeometry();
    move(rect.center()-QPoint(width()/2,height()/2));

    // 系统托盘
    mySystemTray->setIcon(QIcon(":/icon/tray"));

    MyMenu *ContextMenu = new MyMenu(this);
    mySystemTray->setContextMenu(ContextMenu);

    QAction *action_systemTray_pre = new QAction(QIcon(":/icon/back"), "上一首");
    QAction *action_systemTray_next = new QAction(QIcon(":/icon/forward"), "下一首");
    QAction *action_systemTray_play = new QAction(QIcon(":/icon/pause"),"暂停");
    QAction *action_systemTray_exit = new QAction(QIcon(":/icon/exit"), "退出");
    action_systemTray_playmode->setIcon(QIcon(":/icon/list_circle"));
    action_systemTray_playmode->setText("列表循环");

    connect(mySystemTray, &QSystemTrayIcon::activated, this, &MainWindow::systemTrayIcon_actived);
    connect(action_systemTray_pre, &QAction::triggered, this, &MainWindow::on_back_button_clicked);
    connect(action_systemTray_next, &QAction::triggered, this, &MainWindow::on_forward_button_clicked);
    connect(action_systemTray_play, &QAction::triggered, this, &MainWindow::on_play_button_clicked);
    connect(action_systemTray_playmode, &QAction::triggered, this, &MainWindow::on_mode_button_clicked);
    connect(action_systemTray_exit, &QAction::triggered, this, &MainWindow::on_close_button_clicked);

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
            action_systemTray_play->setIcon(QIcon(":/icon/pause"));
            action_systemTray_play->setText("暂停");

        }
        else
        {
            ui->play_button->setIcon(QIcon(":/icon/play"));
            action_systemTray_play->setIcon(QIcon(":/icon/play"));
            action_systemTray_play->setText("播放");
        }
    });

    connect(action_systemTray_playmode, &QAction::triggered, this, [=]{
        if (circleMode == OrderList)
        {
            circleMode = SingleList;
            ui->mode_button->setIcon(QIcon(":/icon/single_circle"));
            action_systemTray_playmode->setIcon(QIcon(":/icon/single_circle"));
            action_systemTray_playmode->setText("单曲循环");
        }
        else if (circleMode == SingleList)
        {
            circleMode = RandomList;
            ui->mode_button->setIcon(QIcon(":icon/random"));
            action_systemTray_playmode->setIcon(QIcon(":/icon/random"));
            action_systemTray_playmode->setText("随机播放");
        }
        else
        {
            circleMode = OrderList;
            ui->mode_button->setIcon(QIcon(":/icon/list_circle"));
            action_systemTray_playmode->setIcon(QIcon(":/icon/list_circle"));
            action_systemTray_playmode->setText("顺序播放");
        }
        settings.setValue("music/mode", circleMode);
    });

    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration){
        ui->playProgressSlider->setMaximum(static_cast<int>(duration));
        if (setPlayPositionAfterLoad)
        {
            player->setPosition(setPlayPositionAfterLoad);
            setPlayPositionAfterLoad = 0;
        }
    });

    connect(expandPlayingButton, SIGNAL(clicked()), this, SLOT(slotExpandPlayingButtonClicked()));
    expandPlayingButton->setFlat(true);
    expandPlayingButton->setFocusPolicy(Qt::NoFocus);
    QString expandPlayingButtonSS(
                "QPushButton"
                "{"
                "   border: none; "
                "focus{padding: -1;}"
                "}");
    expandPlayingButton->setStyleSheet(expandPlayingButtonSS);
    expandPlayingButton->show();

    connect(ui->lyricWidget, SIGNAL(signalAdjustLyricTime(QString)), this, SLOT(adjustCurrentLyricTime(QString)));

    bool lyricStack = settings.value("music/lyricStream", false).toBool();
    if (lyricStack)
        ui->stackedWidget->setCurrentWidget(ui->lyricsPage);

    // 生成真正的随机数
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);

    themeColor = settings.value("music/themeColor", themeColor).toBool();
    blurBg = settings.value("music/blurBg", blurBg).toBool();
    blurAlpha = settings.value("music/blurAlpha", blurAlpha).toInt();

    // 读取数据
    ui->stackedWidget->setCurrentIndex(settings.value("stackWidget/pageIndex").toInt());
    if(settings.value("music/loginState").toBool())
        ui->logo_button->setText(settings.value("music/username").toString());
    else
        ui->logo_button->setText("QooMusic");
    restoreSongList("music/order", orderSongs);
    restoreSongList("music/local", localSongs);
    restoreSongList("music/favorite", favoriteSongs);
    restorePlayList("playlist/list", PLAYLIST);
    setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
    ui->UsernameLabel->setText(settings.value("music/username").toString());
    loginState = settings.value("music/loginState").toBool();
    if (loginState)
    {
       ui->SyncButton->setEnabled(true);
       ui->LogoutButton->setEnabled(true);
       setPlaylistTab(PLAYLIST, ui->tabWidget);
    }
    else
    {
        ui->SyncButton->setEnabled(false);
        ui->LogoutButton->setEnabled(false);
    }


    // 音量
    int volume = settings.value("music/volume", 50).toInt();
    bool mute = settings.value("music/mute", false).toBool();
    if (mute)
    {
        volume = 0;

        ui->sound_button->setIcon(QIcon(":/icon/mute"));

    }
    player->setVolume(volume);
    musicFileDir.mkpath(musicFileDir.absolutePath());

    // 进度条
    Music currentSong = Music::fromJson(settings.value("music/currentSong").toJsonObject());
    qDebug()<<currentSong.simpleString();
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

    // 播放模式
    circleMode = static_cast<PlayCirecleMode>(settings.value("music/mode", 0).toInt());
    if (circleMode == OrderList)
    {
        ui->mode_button->setIcon(QIcon(":icon/list_circle"));
        action_systemTray_playmode->setIcon(QIcon(":/icon/list_circle"));
        action_systemTray_playmode->setText("顺序播放");
    }
    else if (circleMode == SingleList)
    {
        ui->mode_button->setIcon(QIcon(":icon/single_circle"));
        action_systemTray_playmode->setIcon(QIcon(":/icon/single_circle"));
        action_systemTray_playmode->setText("单曲循环");
    }
    else
    {
        ui->mode_button->setIcon(QIcon(":icon/random"));
        action_systemTray_playmode->setIcon(QIcon(":/icon/random"));
        action_systemTray_playmode->setText("随机播放");
    }

    connectDesktopLyricSignals();

    bool showDeskTopLyric = settings.value("music/desktopLyric", false).toBool();

    if (showDeskTopLyric)
    {
        desktopLyric->show();
        ui->lyric_button->setIcon(QIcon(":/icon/lyric"));
    }
    else
    {
        desktopLyric->hide();
        ui->lyric_button->setIcon(QIcon(":/icon/hide"));
    }

    // 数据库初始化
    initSqlite();

    ContextMenu->addAction(music_info);
    ContextMenu->addSeparator();
    ContextMenu->addAction(action_systemTray_pre);
    ContextMenu->addAction(action_systemTray_next);
    ContextMenu->addAction(action_systemTray_play);
    ContextMenu->addAction(action_systemTray_playmode);
    ContextMenu->addSeparator();
    ContextMenu->addAction(action_systemTray_exit);

    mySystemTray->show();
}


MainWindow::~MainWindow()
{
    delete ui;
    desktopLyric->deleteLater();
}

void MainWindow::initSqlite()
{

}

void MainWindow::systemTrayIcon_actived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if (isHidden())
        {
            show();
        }
        else
        {
            hide();
        }
        break;
    default:
        break;
    }
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

    int index = orderSongs.indexOf(playingSong);

    qDebug()<<"歌单index："<<index;

    if (circleMode == RandomList)
    {
        if (!orderSongs.size())
            return ;
        // 添加了下一首播放
        if (onMusicAppendRandom)
        {
           startPlaySong(orderSongs.at(index + 1));
           onMusicAppendRandom--;
           return ;
        }
        else
        {
           int r = qrand() % orderSongs.size();
           if (r != index)
           startPlaySong(orderSongs.at(r));
           return ;
        }
    }

    // 最后一首
    if (index == orderSongs.size() - 1)
    {
        /*    // 播放列表结束 随机播放我的喜欢里的歌 留着做心动模式叭
        if(!localSongs.size())
            return ;

        int r = qrand() % localSongs.size();
        startPlaySong(localSongs.at(r));
        return ;*/
        // 跳到开头播放
        startPlaySong(orderSongs.first());
        index = orderSongs.indexOf(playingSong);
        return ;

    }

    // 歌单里不存在这首歌(放的时候被删了之类的。。
    if(index == -1)
    {
        player->stop();
        return ;
    }

    // Music music = orderSongs.first();
    Music music = orderSongs.at(index+1);
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
        if (musics.contains(playingSong))
        {
            if (orderSongs.size() == 1)
            {
                settings.setValue("music/currentSong", "");
                ui->playingNameLabel->clear();
                ui->playingArtistLabel->clear();
                ui->playingCoverLablel->clear();
                player->stop();
            }
            else
            {
                playNext();
            }
        }
        if (circleMode == RandomList)
            onMusicAppendRandom--;
        orderSongs.removeOne(music);
    }

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
            onMusicAppendRandom++;
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

void MainWindow::appendMusicToPlayList(SongList musics, int row)
{
    foreach (Music music, musics)
    {
        if (PLAYLIST.at(row).contiansMusic.contains(music))
            continue ;
        PLAYLIST[row].contiansMusic.append(music);
//        qDebug()<<"11"<<PLAYLIST.at(row).contiansMusic.size();
    }

    savePlayList("playlist/list", PLAYLIST);
}

void MainWindow::ClearPlayList()
{
//    removeOrderSongs(orderSongs);
    settings.setValue("music/currentSong", "");
    ui->playingNameLabel->clear();
    ui->playingArtistLabel->clear();
    ui->playingCoverLablel->clear();
    player->stop();
    orderSongs.clear();
    saveSongList("music/order", orderSongs);
    setPlayListTable(orderSongs, ui->MusicTable);
}

void MainWindow::setCurrentCover(const QPixmap &pixmap)
{
    currentCover = pixmap;
    if (themeColor)
        setThemeColor(currentCover);
    if (blurBg)
        setBlurBackground(currentCover);


}

void MainWindow::setCurrentLyric(QString lyric)
{
    desktopLyric->setLyric(lyric);
    ui->lyricWidget->setLyric(lyric);
}

void MainWindow::adjustExpandPlayingButton()
{
    QRect rect(ui->playingCoverLablel->mapTo(this, QPoint(0, 0)),
               QSize((ui->playingCoverLablel->width() + ui->playingNameLabel->width()),ui->playingCoverLablel->height()));
    expandPlayingButton->setGeometry(rect);
    expandPlayingButton->raise();
}

void MainWindow::adjustCurrentLyricTime(QString lyric)
{
    if (!playingSong.isValid())
        return;

    QFile file(lyricPath(playingSong));
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << lyric;
    file.flush();
    file.close();

    desktopLyric->setLyric(lyric);
    desktopLyric->setPosition(player->position());
}

void MainWindow::setBlurBackground(const QPixmap &bg)
{
   if (bg.isNull())
       return ;

   // 当前图片变为上一张图
   prevBgAlpha = currentBgAlpha;
   prevBlurBg = currentBlurBg;

   // 开始模糊
   const int radius = qMax(20, qMin(width(), height()) / 5);
   QPixmap pixmap = bg;
   pixmap = pixmap.scaled(this->width() + radius* 2, this->height() + radius * 2,
                          Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
   QImage img = pixmap.toImage();
   QPainter painter(&pixmap);
   qt_blurImage(&painter, img, radius, true, false);

   // 裁剪边缘
   int c = qMin(bg.width(), bg.height());
   c = qMin(c / 2, radius);
   QPixmap clip = pixmap.copy(c, c, pixmap.width() - c * 2, pixmap.height() - c * 2);

   // 抽样获取背景， 设置透明度
   qint64 rgbSum = 0;
   QImage image = clip.toImage();
   int w = image.width(), h = image.height();
   const int m = 16;
   for (int y = 0; y < m; y++)
   {
       for (int x = 0; x < m; x++)
       {
           QColor c = image.pixelColor(w * x / m, h * x / m);
           rgbSum += c.red() + c.green() +c.blue();
       }
   }
   int addin = rgbSum * blurAlpha / (255 * 3 * m * m);
   // 半透明
   currentBlurBg = clip;
   currentBgAlpha = qMin(255, blurAlpha +addin);

   // 出现动画
   QPropertyAnimation* ani1 = new QPropertyAnimation(this, "appearBgProg");
   ani1->setStartValue(0);
   ani1->setEndValue(currentBgAlpha);
   ani1->setDuration(1000);
   ani1->setEasingCurve(QEasingCurve::OutCubic);
   connect(ani1, &QPropertyAnimation::valueChanged, this, [=](const QVariant& val){
      update();
   });

   connect(ani1, &QPropertyAnimation::finished, this, [=]{
      ani1->deleteLater();
   });
   currentBgAlpha = 0;
   ani1->start();

   // 消失动画
   QPropertyAnimation* ani2 = new QPropertyAnimation(this, "disappearBgProg");
   ani2->setStartValue(prevBgAlpha);
   ani2->setEndValue(0);
   ani2->setDuration(1000);
   ani2->setEasingCurve(QEasingCurve::OutCubic);
   connect(ani2, &QPropertyAnimation::valueChanged, this, [=](const QVariant& val){
       prevBgAlpha = val.toInt();
       update();
   });
   connect(ani2, &QPropertyAnimation::finished, this, [=]{
       prevBlurBg = QPixmap();
       ani2->deleteLater();
       update();
   });
   ani2->start();
}

void MainWindow::setThemeColor(const QPixmap &cover)
{
    QColor bg, fg, sbg, sfg;
    auto colors = ImageUtil::extractImageThemeColors(cover.toImage(), 7);
    ImageUtil::getBgFgSgColor(colors, &bg, &fg, &sbg, &sfg);

    prevPa = BFSColor::fromPalette(palette());
    currentPa = BFSColor(QList<QColor>{bg, fg, sbg, sfg});

    QPropertyAnimation* ani = new QPropertyAnimation(this, "paletteProg");
    ani->setStartValue(0);
    ani->setEndValue(1.0);
    ani->setDuration(500);
    connect(ani, &QPropertyAnimation::valueChanged, this, [=](const QVariant& val){
        double d = val.toDouble();
        BFSColor bfs = prevPa + (currentPa - prevPa) * d;
        QColor bg, fg, sbg, sfg;
        bfs.toColors(&bg, &fg, &sbg, &sfg);

        QPalette pa;
        pa.setColor(QPalette::Window, bg);
        pa.setColor(QPalette::Background, bg);
        pa.setColor(QPalette::Button, bg);

        pa.setColor(QPalette::Foreground, fg);
        pa.setColor(QPalette::Text, fg);
        pa.setColor(QPalette::ButtonText, fg);
        pa.setColor(QPalette::WindowText, fg);
        pa.setColor(QPalette::HighlightedText, fg);

        pa.setColor(QPalette::Highlight, sbg);

        QApplication::setPalette(pa);
        setPalette(pa);

        ui->lyricWidget->setColors(sfg, fg);
        desktopLyric->setColors(sfg, fg);
        ui->playingNameLabel->setPalette(pa);
        ui->logo_button->setPalette(pa);
    });
    connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
    ani->start();
}

void MainWindow::connectDesktopLyricSignals()
{
    connect(desktopLyric, &DesktopLyricWidget::signalhide, this, [=]{
        ui->lyric_button->setIcon(QIcon(":/icon/hide"));
        settings.setValue("music/desktopLyric", false);
    });
    connect(desktopLyric, &DesktopLyricWidget::signalSWitchTrans, this, [=]{
        desktopLyric->close();
        desktopLyric->deleteLater();
        desktopLyric = new DesktopLyricWidget(nullptr);
        connectDesktopLyricSignals();
        desktopLyric->show();

        if (playingSong.isValid())
        {
            Music song = playingSong;
            if (QFileInfo(lyricPath(song)).exists())
            {
                QFile file(lyricPath(song));
                file.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream stream(&file);
                QString lyric;
                QString line;
                while (!stream.atEnd())
                {
                    line = stream.readLine();
                    lyric.append(line+"\n");
                }
                file.close();

                desktopLyric->setLyric(lyric);
                desktopLyric->setPosition(player->position());
            }
        }
    });
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
    setPlayListTable(favoriteSongs, ui->FavoriteMusicTable);
}

void MainWindow::removeFavorite(SongList musics)
{
    foreach (Music music, musics)
    {
        if (favoriteSongs.removeOne(music))
        {
            qDebug() << "取消收藏：" << music.simpleString();
        }
    }
    saveSongList("music/favorite", favoriteSongs);
    setPlayListTable(favoriteSongs, ui->FavoriteMusicTable);
}

void MainWindow::saveSongList(QString key, const SongList &songs)
{
    QJsonArray array;
    foreach(Music music, songs)
        array.append(music.toJson());
    settings.setValue(key,array);

}

void MainWindow::savePlayList(QString key, const PlayListList &playlistlist)
{
    QJsonArray array;
    foreach(PlayList pl, playlistlist)
    {
        array.append(pl.toJson());
    }
    settings.setValue(key, array);
}

void MainWindow::restoreSongList(QString key, SongList &songs)
{
    QJsonArray array = settings.value(key).toJsonArray();
    foreach(QJsonValue val, array)
        songs.append(Music::fromJson(val.toObject()));
}

void MainWindow::restorePlayList(QString key, PlayListList &playlistlist)
{
    QJsonArray array = settings.value(key).toJsonArray();
    foreach(QJsonValue val, array)
        playlistlist.append(PlayList::fromJson(val.toObject()));
}

void MainWindow::setAppearBgProg(int x)
{
    this->currentBgAlpha = x;
}

void MainWindow::setDisappearBgProg(int x)
{
    this->prevBgAlpha = x;
}

void MainWindow::setPaletteBgProg(double x)
{
    this->paletteAlpha = x;
}

void MainWindow::setLyricScroll(int x)
{
    this->lyricScroll = x;
}

int MainWindow::getLyricScroll() const
{
    return this->lyricScroll;
}

int MainWindow::getAppearBgProg() const
{
    return this->currentBgAlpha;
}

int MainWindow::getDisappearBgProg() const
{
    return this->prevBgAlpha;
}

double MainWindow::getPaletteBgProg() const
{
   return paletteAlpha;
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

void MainWindow::setPlayListTable(QList<Music> songs, QTableWidget *table)
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

void MainWindow::setPLAYLISTTable(PlayListList playlist, QListWidget *list)
{
    ui->MusiclistWidget->clear();

    for (int index = 0; index < playlist.size(); index++)
    {
        ui->MusiclistWidget->addItem(playlist.at(index).name);
    }
}

void MainWindow::setPlaylistTab(PlayListList pl, QTabWidget *tab)
{
    ui->tabWidget->clear();
    for (int index = 0; index < pl.size(); index++)
    {
        QListView *QLV = new QListView(tab);
        tab->addTab(QLV, pl.at(index).name);
        QLV->setContextMenuPolicy(Qt::CustomContextMenu);
        QLV->setEditTriggers(QAbstractItemView::NoEditTriggers);
        connect(QLV, &QListView::customContextMenuRequested, this, [=](QPoint){
            ShowPlaylistTabMenu(QLV);
        });
        QStringList sl;
        foreach (Music music, pl.at(index).contiansMusic)
        {
            sl << music.simpleString();
        }
        QAbstractItemModel *model = QLV->model();
        if (model)
            delete model;
        model = new QStringListModel(sl);
        QLV->setModel(model);
    }
}

/**
 * 立即开始播放音乐
 */
void MainWindow::playLocalSong(Music music)
{
    qDebug() << "开始播放" << music.simpleString();
    mySystemTray->setToolTip(music.simpleString());
    QString mi = music.simpleString();
    if (mi.length() > 6)
    {
        mi = mi.left(5) + "...";
        music_info->setText(mi);
        music_info->setToolTip(music.simpleString());
    }
    else
        music_info->setText(mi);
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

    // 设置
    if (QFileInfo(lyricPath(music)).exists())
    {
        QFile file(lyricPath(music));
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream stream(&file);
        QString lyric;
        QString line;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            lyric.append(line + "\n");
        }

        file.close();

        setCurrentLyric(lyric);
    }
    else
    {
        setCurrentLyric("");
        downloadSongLyric(music);
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

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePosition = event->pos();
    if(mousePosition.y() <= (ui->widget_3->pos().y() + ui->widget_3->geometry().bottom()))
    {
        qDebug()<<"position"<<mousePosition.y();
        if(windowState() == Qt::WindowMaximized)
        {
        on_normal_button_clicked();
        }
        else
        on_max_button_clicked();
    }
}

void MainWindow::showEvent(QShowEvent *)
{
   restoreGeometry(settings.value("qoomusicwindow/geometry").toByteArray());
   restoreState(settings.value("qoomusicwindow/state").toByteArray());

   adjustExpandPlayingButton();
   if (settings.value("music/desktopLyric", false).toBool())
       desktopLyric->show();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    settings.setValue("music/currentSong",playingSong.toJson());
    settings.setValue("qoomusicwindow/state", this->saveState());
    settings.setValue("qoomusicwindow/geometry", this->saveGeometry());
    settings.setValue("music/playPosition", player->position());

    if (!desktopLyric->isHidden())
        desktopLyric->close();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    adjustExpandPlayingButton();

    tip_box->adjustPosition();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QMainWindow::paintEvent(e);

    if (blurBg)
    {
        QPainter painter(this);
        if (!currentBlurBg.isNull())
        {
            painter.setOpacity((double)currentBgAlpha / 255);
            painter.drawPixmap(rect(), currentBlurBg);
        }
        if (!prevBlurBg.isNull() && prevBgAlpha)
        {
            painter.setOpacity((double)prevBgAlpha / 255);
            painter.drawPixmap(rect(), prevBlurBg);
        }
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
    settings.setValue("music/currentSong",playingSong.toJson());
//    close();
     QCoreApplication::quit();
}

void MainWindow::on_max_button_clicked()
{
    if(windowState() == Qt::WindowMaximized)
    {
        showNormal();
        ui->max_button->setIcon(QIcon(":/icon/max"));
    }
    else
    {
        setWindowState(Qt::WindowMaximized);
        ui->max_button->setIcon(QIcon(":/icon/normal"));
    }
}

void MainWindow::on_min_button_clicked()
{
    //最小化到托盘
    if(!mySystemTray->isVisible()){
        mySystemTray->show();
    }
    hide();
//    event->ignore();
//    setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_normal_button_clicked()
{
    setWindowState(Qt::WindowMinimized);
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
        // todo:搜索歌单
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
        QMenu *PlayListMenu = new QMenu("添加到歌单", this);
        if (favoriteSongs.contains(currentsong))
            Favorite->setText("从我的喜欢中移除");
        else
            Favorite->setText("添加到我的喜欢");

        if (!PLAYLIST.size())
            PlayListMenu->setEnabled(false);
        else
            PlayListMenu->setEnabled(true);

        // QAction对象添加到菜单上
        menu->addAction(playNow);
        menu->addAction(playNext);
        menu->addAction(addToPlayList);
        menu->addAction(Favorite);
        menu->addMenu(PlayListMenu);

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

        for (int index = 0; index < PLAYLIST.size(); index++)
        {
            QAction* plm = new QAction(PLAYLIST.at(index).name, this);
            PlayListMenu->addAction(plm);
            connect(plm, &QAction::triggered, [=]{
                appendMusicToPlayList(musics, index);
            });
        }

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
    QAction *clearPlayList = new QAction("清空列表", this);
    QMenu *PlayListMenu = new QMenu("添加到歌单", this);
    if (favoriteSongs.contains(currentsong))
        Favorite->setText("从我的喜欢中移除");
    else
        Favorite->setText("添加到我的喜欢");

    if (!orderSongs.size())
        clearPlayList->setEnabled(false);
    else
        clearPlayList->setEnabled(true);

    if (!PLAYLIST.size())
        PlayListMenu->setEnabled(false);
    else
        PlayListMenu->setEnabled(true);

    menu->addAction(playNow);
    menu->addAction(playNext);
    menu->addAction(removeToPlayList);
    menu->addAction(Favorite);
    menu->addAction(clearPlayList);
    menu->addMenu(PlayListMenu);

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

    connect(clearPlayList, &QAction::triggered, [=]{
        ClearPlayList();
    });

    for (int index = 0; index < PLAYLIST.size(); index++)
    {
        QAction* plm = new QAction(PLAYLIST.at(index).name, this);
        PlayListMenu->addAction(plm);
        connect(plm, &QAction::triggered, [=]{
            appendMusicToPlayList(musics, index);
        });
    }

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
    QMenu *PlayListMenu = new QMenu("添加到歌单", this);

    if (favoriteSongs.contains(currentsong))
        Favorite->setText("从我的喜欢中移除");
    else
        Favorite->setText("添加到我的喜欢");

    if (!PLAYLIST.size())
        PlayListMenu->setEnabled(false);
    else
        PlayListMenu->setEnabled(true);

    menu->addAction(playNow);
    menu->addAction(playNext);
    menu->addAction(addToPlayList);
    menu->addAction(Favorite);
    menu->addMenu(PlayListMenu);

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

    for (int index = 0; index < PLAYLIST.size(); index++)
    {
        QAction* plm = new QAction(PLAYLIST.at(index).name, this);
        PlayListMenu->addAction(plm);
        connect(plm, &QAction::triggered, [=]{
            appendMusicToPlayList(musics, index);
        });
    }
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
    if (circleMode == OrderList)
    {
    int index = orderSongs.indexOf(playingSong);
    Music music = orderSongs.at(index);
    settings.setValue("music/currentSong",music.toJson());
    ui->playingNameLabel->clear();
    ui->playingArtistLabel->clear();
    ui->playingCoverLablel->clear();

    playNext();
    }
    else if (circleMode == SingleList)
    {
        // 不用管，会自己放下去
        player->setPosition(0);
        player->play();
    }
    else if (circleMode == RandomList)
    {
        int index = orderSongs.indexOf(playingSong);
        Music music = orderSongs.at(index);
        settings.setValue("music/currentSong",music.toJson());
        ui->playingNameLabel->clear();
        ui->playingArtistLabel->clear();
        ui->playingCoverLablel->clear();

        playNext();
    }
}

/**
 * 暂停、播放
 */
void MainWindow::on_play_button_clicked()
{
    if (!orderSongs.size())
        return ;
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
   if (desktopLyric && !desktopLyric->isHidden())
       desktopLyric->setPosition(position);
   if (ui->lyricWidget->setPosition(position))
   {
       QPropertyAnimation* ani = new QPropertyAnimation(this, "lyricScroll");
       ani->setStartValue(ui->scrollArea->verticalScrollBar()->sliderPosition());
       ani->setEndValue(qMax(0, ui->lyricWidget->getCurrentTop() - this->height() / 2));
       ani->setDuration(200);
       connect(ani, &QPropertyAnimation::valueChanged, this, [=]{
           ui->scrollArea->verticalScrollBar()->setSliderPosition(lyricScroll);
       });
       connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
       ani->start();
   }
   ui->playProgressSlider->setSliderPosition(static_cast<int>(position));
   update();
}

void MainWindow::on_playProgressSlider_sliderMoved(int position)
{
    player->setPosition(position);
}


void MainWindow::on_mode_button_clicked()
{
    if(circleMode == OrderList)
    {
        circleMode = SingleList;
        ui->mode_button->setIcon(QIcon(":/icon/single_circle"));
        action_systemTray_playmode->setIcon(QIcon(":/icon/single_circle"));
        action_systemTray_playmode->setText("单曲循环");
    }
    else if (circleMode == SingleList)
    {
        circleMode = RandomList;
        ui->mode_button->setIcon(QIcon(":icon/random"));
        action_systemTray_playmode->setIcon(QIcon(":/icon/random"));
        action_systemTray_playmode->setText("随机播放");
    }
    else
    {
        circleMode = OrderList;
        ui->mode_button->setIcon(QIcon(":/icon/list_circle"));
        action_systemTray_playmode->setIcon(QIcon(":/icon/list_circle"));
        action_systemTray_playmode->setText("顺序播放");
    }
    settings.setValue("music/mode", circleMode);
}

void MainWindow::on_forward_button_clicked()
{
    if (!orderSongs.size())
        return ;
    SlotSongPlayEnd();
}

void MainWindow::slotExpandPlayingButtonClicked()
{
    // 隐藏歌词
    if (ui->stackedWidget->currentWidget() == ui->lyricsPage)
    {
        QRect rect(ui->stackedWidget->mapTo(this, QPoint(5, 0)), ui->stackedWidget->size() - QSize(5, 0));
        QPixmap pixmap(rect.size());
        render(&pixmap, QPoint(0, 0), rect);
        QLabel* label = new QLabel(this);
        label->setScaledContents(true);
        label->setGeometry(rect);
        label->setPixmap(pixmap);
        QPropertyAnimation* ani = new QPropertyAnimation(label, "geometry");
        ani->setStartValue(rect);
        ani->setEndValue(QRect(ui->playingCoverLablel->geometry().center(), QSize(1, 1)));
        ani->setEasingCurve(QEasingCurve::InOutCubic);
        ani->setDuration(300);
        connect(ani, &QPropertyAnimation::finished, this, [=]{
            ani->deleteLater();
            label->deleteLater();
        });
        label->show();
        ani->start();
        ui->stackedWidget->setCurrentWidget(ui->searchResultPage);
        settings.setValue("music/lyricStream", false);
    }
    else // 显示歌词
    {
        ui->stackedWidget->setCurrentWidget(ui->lyricsPage);
        QRect rect(ui->stackedWidget->mapTo(this, QPoint(5, 0)), ui->stackedWidget->size()-QSize(5, 0));
        QPixmap pixmap(rect.size());
        render(&pixmap, QPoint(0, 0), rect);
        QLabel* label = new QLabel(this);
        label->setScaledContents(true);
        label->setGeometry(rect);
        label->setPixmap(pixmap);
        QPropertyAnimation* ani = new QPropertyAnimation(label, "geometry");
        ani->setStartValue(QRect(ui->playingCoverLablel->geometry().center(), QSize(1,1)));
        ani->setEndValue(rect);
        ani->setDuration(300);
        ani->setEasingCurve(QEasingCurve::InOutCubic);
        ui->stackedWidget->setCurrentWidget(ui->searchResultPage);
        connect(ani, &QPropertyAnimation::finished, this, [=]{
            ui->stackedWidget->setCurrentWidget(ui->lyricsPage);
            ani->deleteLater();
            label->deleteLater();
        });
        label->show();
        ani->start();
        settings.setValue("music/lyricStream", true);
    }
}

void MainWindow::on_back_button_clicked()
{
    if (!orderSongs.size())
        return ;
}

void MainWindow::on_MusicTable_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = ui->MusicTable->row(item);
    Music currentsong;
    if (row > -1)
        currentsong = orderSongs.at(row);
    startPlaySong(currentsong);
}

void MainWindow::on_lyric_button_clicked()
{
    bool showDesktopLyric = !settings.value("music/desktopLyric", false).toBool();
    settings.setValue("music/desktopLyric", showDesktopLyric);
    if (showDesktopLyric)
    {
        desktopLyric->show();
        ui->lyric_button->setIcon(QIcon(":/icon/lyric"));
    }
    else
    {
        desktopLyric->hide();
        ui->lyric_button->setIcon(QIcon(":/icon/hide"));
    }
}

void MainWindow::on_my_favorite_button_clicked()
{
    setPlayListTable(favoriteSongs, ui->FavoriteMusicTable);
    ui->stackedWidget->setCurrentWidget(ui->Favoritepage);
}

void MainWindow::on_FavoriteMusicTable_customContextMenuRequested(const QPoint &pos)
{
    auto items = ui->FavoriteMusicTable->selectedItems();

    QList<Music> musics;
    foreach (auto item, items)
    {
        int row = ui->FavoriteMusicTable->row(item);
        int col = ui->FavoriteMusicTable->column(item);
        if (col != 0)
            continue;
        musics.append(favoriteSongs.at(row));
    }
    int row = ui->FavoriteMusicTable->currentRow();
    Music currentsong;
    if (row > -1)
        currentsong = favoriteSongs.at(row);

    QMenu* menu = new QMenu(this);

    QAction *playNow = new QAction("立即播放", this);
    QAction *playNext = new QAction("下一首播放", this);
    QAction *addToPlayList = new QAction("添加到播放列表", this);
    QAction *Favorite = new QAction("从我的喜欢中移除", this);
    QMenu *PlayListMenu = new QMenu("添加到歌单", this);
//    if (favoriteSongs.contains(currentsong))
//        Favorite->setText("从我的喜欢中移除");
//    else
//        Favorite->setText("添加到我的喜欢");

    menu->addAction(playNow);
    menu->addAction(playNext);
    menu->addAction(addToPlayList);
    menu->addAction(Favorite);
    menu->addMenu(PlayListMenu);

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
//        if(!favoriteSongs.contains(currentsong))
//            addFavorite(musics);
//        else
            removeFavorite(musics);

    });

    for (int index = 0; index < PLAYLIST.size(); index++)
    {
        QAction* plm = new QAction(PLAYLIST.at(index).name, this);
        PlayListMenu->addAction(plm);
        connect(plm, &QAction::triggered, [=]{
            appendMusicToPlayList(musics, index);
        });
    }
    // 显示菜单
    menu->exec(cursor().pos());

    // 释放内存
    QList<QAction*> list = menu->actions();
    foreach(QAction* action, list)
        delete action;
    delete menu;
}

void MainWindow::on_FavoriteMusicTable_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = ui->FavoriteMusicTable->row(item);
    Music currentsong;
    if (row > -1 )
        currentsong = favoriteSongs.at(row);
    if (orderSongs.contains(currentsong))
    {
        orderSongs.removeOne(currentsong);
        setPlayListTable(orderSongs, ui->MusicTable);
    }
    else
        orderSongs.insert(0, currentsong);
    startPlaySong(currentsong);
}

void MainWindow::on_my_song_list_button_clicked()
{
//    savePlayList("playlist/list", PLAYLIST);
    setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
}

void MainWindow::on_add_list_Button_clicked()
{
    bool ok;
    QString text =QInputDialog::getText(this, "新建歌单", "输入新歌单名字", QLineEdit::Normal, "", &ok);
    // 判断歌名是否太长
    if (text.length() > 10)
    {
      QMessageBox::warning(this, "警告:歌名太长啦！！！", "歌单名字最多十个字");
      return ;
    }
    else
    {
        if (ok && !text.isEmpty())
        {
            // 歌单列表是否为空
            if (PLAYLIST.size() == 0)
            {
                PlayList templist;
                templist.name = text;
                PLAYLIST.push_back(templist);
            }
            else
            {
                for (int i = 0; i < PLAYLIST.size(); i++)
                {
                    if (PLAYLIST.at(i).name == text)
                    {
                        QMessageBox::warning(this, "警告:歌单已存在", "歌单已存在");
                        return ;
                    }
                }
                PlayList templist;
                templist.name = text;
                PLAYLIST.push_back(templist);
            }
        }
    }
    savePlayList("playlist/list", PLAYLIST);
    setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
}

void MainWindow::on_MusiclistWidget_customContextMenuRequested(const QPoint &pos)
{
    auto items = ui->MusiclistWidget->selectedItems();
    // list里的所有歌单
    QList<PlayList> playlistscout;
    playlistscout.append(PLAYLIST);

    int row = ui->MusiclistWidget->currentRow();
    PlayList currentplaylist;
    if (row < -1)
        currentplaylist= PLAYLIST.at(row);

    QMenu *menu = new QMenu(this);

    QAction *playNow = new QAction("播放歌单", this);
    QAction *addToPlayList = new QAction("添加到播放列表", this);
    QAction *rename = new QAction("重命名", this);
    QAction *deletePlayList = new QAction("删除歌单", this);

    menu->addAction(playNow);
    menu->addAction(addToPlayList);
    menu->addAction(rename);
    menu->addAction(deletePlayList);

    connect(playNow, &QAction::triggered, [=]{
       Play_Playlist(items);
    });

    connect(addToPlayList, &QAction::triggered, [=]{
        // 添加进播放列表
        QList<PlayList> playlists1 = playlistscout;

        foreach (auto item, items)
        {
            // 选中的歌单
            int row1 = ui->MusiclistWidget->row(item);
            foreach(Music music, playlists1[row1].contiansMusic)
            {
                if (orderSongs.contains(music))
                    playlists1[row1].contiansMusic.removeOne(music);
            }
            orderSongs.append(playlists1.at(row1).contiansMusic);

        }
        if (isNotPlaying() && orderSongs.size())
        {
            qDebug() << "当前未播放， 开始播放列表";
            startPlaySong(orderSongs.first());
        }

        saveSongList("music/order", orderSongs);
        setPlayListTable(orderSongs, ui->MusicTable);
    });

    connect(rename, &QAction::triggered, [=]{
        bool ok;
        QString text = QInputDialog::getText(this, "重命名歌单", "请输入歌单新名字", QLineEdit::Normal, "", &ok);
        if (text.length() > 10)
        {
          QMessageBox::warning(this, "警告:歌名太长啦！！！", "歌单名字最多十个字");
          return ;
        }
        else
        {
            if (ok && !text.isEmpty())
            {
                for (int i = 0; i < PLAYLIST.size(); i++)
                {
                    if (PLAYLIST.at(i).name == text)
                    {
                        QMessageBox::warning(this, "警告:歌单名已存在", "请重新取名");
                        return ;
                    }
                }
                PLAYLIST[row].name = text;
            }
        }
        savePlayList("playlist/list", PLAYLIST);
        setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
    });

    connect(deletePlayList, &QAction::triggered, [=]{
        foreach (auto item, items)
        {
            // 选中的歌单
            int row1 = ui->MusiclistWidget->row(item);
            ui->MusiclistWidget->takeItem(row1);
            PLAYLIST.removeAt(row1);
        }
        savePlayList("playlist/list", PLAYLIST);
        setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
    });

    // 显示菜单
    menu->exec(cursor().pos());

    // 释放内存
    QList<QAction*> list = menu->actions();
    foreach(QAction* action, list)
        delete action;
    delete menu;
}

void MainWindow::Play_Playlist(QList<QListWidgetItem *> items)
{
    ClearPlayList();
     // 选择歌单开始播放
    foreach (auto item, items)
    {
        // 选中的歌单
        int row1 = ui->MusiclistWidget->row(item);
        orderSongs.append(PLAYLIST.at(row1).contiansMusic);
    }
     startPlaySong(orderSongs.first());

     saveSongList("music/order", orderSongs);
     setPlayListTable(orderSongs, ui->MusicTable);
}



void MainWindow::on_MusiclistWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QList<QListWidgetItem *> items = ui->MusiclistWidget->selectedItems();
    Play_Playlist(items);
}

void MainWindow::on_MusiclistWidget_itemClicked(QListWidgetItem *item)
{
    int index = ui->MusiclistWidget->row(item);
    setPlayListTable(PLAYLIST.at(index).contiansMusic, ui->PlayListTable);
    ui->stackedWidget->setCurrentWidget(ui->playListpage);
}

void MainWindow::on_PlayListTable_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = ui->PlayListTable->row(item);
    int index = ui->MusiclistWidget->currentRow();
    Music currentsong;
    if (row > -1 )
        currentsong = PLAYLIST.at(index).contiansMusic.at(row);
    if (orderSongs.contains(currentsong))
    {
        orderSongs.removeOne(currentsong);
        setPlayListTable(orderSongs, ui->MusicTable);
    }
    else
        orderSongs.insert(0, currentsong);
    startPlaySong(currentsong);
}

void MainWindow::on_PlayListTable_customContextMenuRequested(const QPoint &pos)
{
    auto items = ui->PlayListTable->selectedItems();
    int index = ui->MusiclistWidget->currentRow();
    QList<Music> musics;
    foreach (auto item, items)
    {
        int row = ui->PlayListTable->row(item);
        int col = ui->PlayListTable->column(item);
        if (col != 0)
            continue;
        musics.append(PLAYLIST.at(index).contiansMusic.at(row));
    }
    int row = ui->PlayListTable->currentRow();
    Music currentsong;
    if (row > -1)
        currentsong = PLAYLIST.at(index).contiansMusic.at(row);

    QMenu* menu = new QMenu(this);

    QAction *playNow = new QAction("立即播放", this);
    QAction *playNext = new QAction("下一首播放", this);
    QAction *addToPlayList = new QAction("添加到播放列表", this);
    QAction *removeToPlayList = new QAction("从歌单中移除", this);

    menu->addAction(playNow);
    menu->addAction(playNext);
    menu->addAction(addToPlayList);
    menu->addAction(removeToPlayList);

    connect(playNow, &QAction::triggered, [=]{
        startPlaySong(currentsong);
    });

    connect(playNext, &QAction::triggered, [=]{
       appendNextSongs(musics);
    });

    connect(addToPlayList, &QAction::triggered, [=]{
        appendOrderSongs(musics);
    });

    connect(removeToPlayList, &QAction::triggered, [=]{
        foreach (Music music, musics)
        {
            if (PLAYLIST[index].contiansMusic.removeOne(music))
            {
                qDebug()<< "从歌单中删除："<< music.simpleString();
            }
        }
        savePlayList("playlist/list", PLAYLIST);
        setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
        ui->MusiclistWidget->setCurrentRow(index);
        setPlayListTable(PLAYLIST.at(index).contiansMusic, ui->PlayListTable);
    });

    // 显示菜单
    menu->exec(cursor().pos());

    // 释放内存
    QList<QAction*> list = menu->actions();
    foreach(QAction* action, list)
        delete action;
    delete menu;
}

void MainWindow::on_logo_button_clicked()
{
    bool LS = settings.value("music/loginState").toBool();
    if(!LS)// LS = false 未登录
    {
        LoginRegisterDialog *lrd = new LoginRegisterDialog(this);
        lrd->show();
        connect(lrd, &LoginRegisterDialog::signalLoginFinished,[=]{
            ui->logo_button->setText(settings.value("music/username").toString());
            ui->UsernameLabel->setText(settings.value("music/username").toString());
//            ui->logo_button->setIcon()
             // 同步歌单
            QString username = settings.value("music/username").toString();
            QString password = settings.value("music/password").toString();
            NetUtil* d = new NetUtil(SP + "upload2.php", QStringList{"username", username, "password", password});
            connect(d, &NetUtil::finished, [=](QString s){
                // 云端歌单列表保存为配置文件
                QJsonArray array = QstringToJson(getXml(s, "PLAYLIST"));
                foreach(QJsonValue val, array)
                    PLAYLIST.append(PlayList::fromJson(val.toObject()));
                savePlayList("playlist/list", PLAYLIST);
                setPlaylistTab(PLAYLIST, ui->tabWidget);
                setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
            });
            QTimer::singleShot(1200, [=]{
                lrd->close();
                ui->stackedWidget->setCurrentWidget(ui->Userpage);
            });
        });
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->Userpage);
        // 打开用户界面
       setPlaylistTab(PLAYLIST, ui->tabWidget);
    }
}

void MainWindow::on_LogoutButton_clicked()
{
    setPlayListTable(localSongs, ui->localMusicTable);
    ui->stackedWidget->setCurrentWidget(ui->localMusicpage);
//    on_SyncButton_clicked();
    bool loginState = false;
    QString password = "";
    QString username = "";
    ui->logo_button->setText("QooMusic");
    settings.setValue("music/username", username);
    settings.setValue("music/loginState", loginState);
    settings.setValue("music/password", password);
    // 清空歌单列表
    PLAYLIST.clear();
    savePlayList("playlist/list", PLAYLIST);
    setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
    emit signalLogout();
}

void MainWindow::on_SyncButton_clicked()
{
    // 上传歌单到服务器
    QString username = settings.value("music/username").toString();
    QString password = settings.value("music/password").toString();
    QJsonArray playlistSync = settings.value("playlist/list").toJsonArray();
    NetUtil* d = new NetUtil(SP + "upload.php", QStringList{"username", username, "password", password, "playlist", JsonToQstring(playlistSync)});
    connect(d, &NetUtil::finished, [=](QString s){
        if (getXml(s, "STATE") == "OK")
        {
            // 同步成功
            NoticeDialog *nd = new NoticeDialog(this);
            nd->OnSyncFinished("同步成功");
            nd->show();
            QTimer::singleShot(1200, [=]{
                nd->close();
            });
        }
        else
        {
            //同步失败
            NoticeDialog *nd = new NoticeDialog(this);
            nd->OnSyncFinished("同步失败"+getXml(s, "REASON"));
            nd->show();
            QTimer::singleShot(1200, [=]{
                nd->close();
            });
        }
    });
}

void MainWindow::ShowPlaylistTabMenu(QListView* QLV)
{
      auto indexs = QLV->selectionModel()->selectedRows(0);
      int currentIndex = ui->tabWidget->currentIndex();   // 当前选择的tab页index
      int row1 = QLV->currentIndex().row();   // 当前选择tab页的第几行
      SongList musics;
      Music currentMusic;
      foreach (auto index, indexs)
          musics.append(PLAYLIST.at(currentIndex).contiansMusic.at(index.row()));
      if (row1 > -1)
        currentMusic = PLAYLIST.at(currentIndex).contiansMusic.at(row1);

      QMenu* menu = new QMenu(this);

      QAction *playNow = new QAction("立即播放", this);
      QAction *playNext = new QAction("下一首播放", this);
      QAction *addToPlayList = new QAction("添加到播放列表", this);
      QAction *removeToPlayList = new QAction("从歌单中移除", this);

      menu->addAction(playNow);
      menu->addAction(playNext);
      menu->addAction(addToPlayList);
      menu->addAction(removeToPlayList);

      connect(playNow, &QAction::triggered, [=]{
          startPlaySong(currentMusic);
      });

      connect(playNext, &QAction::triggered, [=]{
         appendNextSongs(musics);
      });

      connect(addToPlayList, &QAction::triggered, [=]{
          appendOrderSongs(musics);
      });

      connect(removeToPlayList, &QAction::triggered, [=]{
          foreach (Music music, musics)
          {
              if (PLAYLIST[currentIndex].contiansMusic.removeOne(music))
              {
                  qDebug()<< "从歌单中删除："<< music.simpleString();
              }
          }
          savePlayList("playlist/list", PLAYLIST);
          setPLAYLISTTable(PLAYLIST, ui->MusiclistWidget);
          setPlaylistTab(PLAYLIST, ui->tabWidget);
      });

      // 显示菜单
      menu->exec(cursor().pos());

      // 释放内存
      QList<QAction*> list = menu->actions();
      foreach(QAction* action, list)
          delete action;
      delete menu;
}

QJsonArray MainWindow::QstringToJson(QString jsonString)
{
//    qDebug()<<"====="<<jsonString;
//    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
//    if(jsonDocument.isNull())
//    {
//        qDebug()<< "String NULL"<< jsonString.toLocal8Bit().data();
//    }
//    QJsonArray jsonArray = jsonDocument.array();

    QJsonDocument document;
    QJsonParseError err;
    document = QJsonDocument::fromJson(jsonString.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError)

    {
            qDebug() << "Parse json " << jsonString.toUtf8() << " error: " << err.error;

      }

    QJsonArray  data(document.array());
    return data;
}

QString MainWindow::JsonToQstring(QJsonArray jsonArray)
{
    return QString(QJsonDocument(jsonArray).toJson());
}
