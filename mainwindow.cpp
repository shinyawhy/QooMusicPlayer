#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

void MainWindow::isSongDownloaded(Music music)
{
    
}

void MainWindow::downloadSong(Music music)
{
//    QString url = API_DOMAIN + "/"
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
