#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    musicFileDir(QApplication::applicationDirPath() + "/musics")
{
    ui->setupUi(this);

    // UI初始化
    initUI();

    // 播放器初始化
    initPlayer();

    // 数据库初始化
    initSqlite();

    // 配置文件初始化
    initSettings();

    // 歌单初始化
    initMusicList();

    setWindowFlags(Qt::FramelessWindowHint);
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

    musicFileDir.mkpath(musicFileDir.absolutePath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{

}

void MainWindow::initUI()
{

}

void MainWindow::initPlayer()
{

}

void MainWindow::initSqlite()
{

}

void MainWindow::initSettings()
{

}

void MainWindow::initMusicList()
{

}

void MainWindow::initMenuAction()
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
                }
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
                pixmap = pixmap.scaledToHeight(ui->playingCoverLable->height());
                ui->playingCoverLable->setPixmap(pixmap);
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
}

void MainWindow::playNext()
{
    // 播放列表结束 随机播放我的喜欢里的歌
    if (!orderSongs.size())
    {
        if(!favoriteSongs.size())
            return ;

        int r = qrand() % favoriteSongs.size();
        startPlaySong(favoriteSongs.at(r));
        return ;
    }

    Music music = orderSongs.takeFirst();
    saveSongList("music/order", orderSongs);
    // TODO setSongModelToView

    startPlaySong(music);
    emit signalOrderSongPlayed(music);

}

void MainWindow::appendOrderSongs(SongList musics)
{

}

void MainWindow::appendNextSongs(SongList musics)
{

}

void MainWindow::setCurrentCover(const QPixmap &pixmap)
{
    currentCover = pixmap;
}

void MainWindow::setCurrentLyric(QString lyric)
{

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

void MainWindow::playLocalSong(Music music)
{

}

void MainWindow::addDownloadSong(Music music)
{

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
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePressed = false;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(mousePosition.y() <= (ui->widget_3->pos().y() + ui->widget_3->geometry().bottom()))
    {
        if(windowState() == Qt::WindowMaximized)
        on_normal_button_clicked();
        else
        on_max_button_clicked();
    }
}
/**
 * 重写窗体缩放， 仅在Windows环境下有效
 */
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)

    MSG *param = static_cast<MSG *>(message);

    switch (param->message) {
    case WM_NCHITTEST:
    {
       int nX = GET_X_LPARAM(param->lParam) - this->geometry().x();
       int nY = GET_Y_LPARAM(param->lParam) - this->geometry().y();
       // 鼠标位于子控件上， 不进行处理
       if (childAt(nX, nY) != NULL && childAt(nX, nY) != ui->centralwidget)
       {
           return QWidget::nativeEvent(eventType, message, result);
       }
       *result = HTCAPTION;

       // 鼠标位于窗体边框， 进行缩放
       if ((nX > 0) && (nX < 5))
           *result = HTLEFT;

       if ((nX > this->width() - 5) && (nX < this->width()))
           *result = HTRIGHT;

       if ((nY > 0) && (nY < 5))
           *result = HTTOP;

       if ((nY > this->height() - 5) && (nY < this->height()))
           *result = HTBOTTOM;

       if ((nX > 0) && (nX < 5) && (nY > 0)
               && (nY < 5))
           *result = HTTOPLEFT;

       if ((nX > this->width() - 5) && (nX < this->width())
               && (nY > 0) && (nY < 5))
           *result = HTTOPRIGHT;

       if ((nX > 0) && (nX < 5)
               && (nY > this->height() - 5) && (nY < this->height()))
           *result = HTBOTTOMLEFT;

       if ((nX > this->width() - 5) && (nX < this->width())
               && (nY > this->height() - 5) && (nY < this->height()))
           *result = HTBOTTOMRIGHT;

       return true;
    }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

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
    QString text = ui->search_edit->text();
    searchMusic(text);
    ui->stackedWidget->setCurrentWidget(ui->searchResultPage);
}
