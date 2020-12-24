#include "desktoplyricwidget.h"

DesktopLyricWidget::DesktopLyricWidget(QWidget *parent) : QWidget(parent),
        settings(QApplication::applicationDirPath()+"/musics.ini", QSettings::Format::IniFormat)
{
    this->setWindowTitle("桌面歌词");
    this->setMinimumSize(45, 15);
    this->setMaximumSize(800, 100);
    if ((jiWindow = settings.value("music/desktopLyricTrans", false).toBool()))
    {
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    }
    else
    {
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        this->setAttribute(Qt::WA_TranslucentBackground, true);
    }
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setMouseTracking(true);



    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu()));

    lineMode = static_cast<LineMode>(settings.value("music/lineMode", lineMode).toInt());
    alignMode = static_cast<AlignMode>(settings.value("music/alignMoed", alignMode).toInt());
    playingColor = qvariant_cast<QColor>(settings.value("music/playingColor", playingColor));
    waitingColor = qvariant_cast<QColor>(settings.value("music/waitingColor", waitingColor));
    bgColor = qvariant_cast<QColor>(settings.value("music/bgColor", bgColor));
    pointSize = settings.value("music/desktopLyricPointSize", pointSize).toInt();

    QFont font = this->font();
    font.setPointSize(pointSize);
    QFontMetrics fm(this->font());
    fontHeight = fm.height();
    lineSpacing = fm.lineSpacing();

    this->setMinimumHeight((fontHeight + boundaryWidth) * 2);
    this->setMinimumWidth((fm.horizontalAdvance("一二三四五六七八九十十一十二")) * 2);

}

void DesktopLyricWidget::setLyric(QString text)
{
    // 检测是不是全是毫秒还是10毫秒的
    int ms10x = 10;
    QRegularExpression re10("\\[\\d{2}:\\d{2}\\.([1-9]\\d{2})\\]");
    QRegularExpressionMatch match10;
    if (text.lastIndexOf(re10, -1, &match10) != -1)
    {
        int val = match10.captured(1).toInt();
        if (val > 0) // 存在不为0的三位数
        {
            ms10x = 1;
        }
    }

    // 遍历每一行
    QStringList sl = text.split("\n", QString::SkipEmptyParts);
    LyricBean prevLyric(false);
    qint64 currentTime = 0;
    lyricStream.clear();
    foreach (QString line, sl)
    {
        QRegularExpression re("^\\[(\\d{2}):(\\d{2})\\.(\\d{2,3})\\](\\[(\\d{2}):(\\d{2})\\.(\\d{2,3})\\])?(.*)$");
        QRegularExpressionMatch match;
        if (line.indexOf(re, 0, &match) == -1)
        {
            LyricBean lyric;
            lyric.start = currentTime;
            lyric.end = 0;
            lyric.text = line;
            lyricStream.append(lyric);
            continue;
        }
        QStringList caps = match.capturedTexts();
        LyricBean lyric;
        int minute = caps.at(1).toInt();
        int second = caps.at(2).toInt();
        int ms10 = caps.at(3).toInt();
        lyric.start = minute * 60000 + second*1000 + ms10 * ms10x;
        if (!caps.at(4).isEmpty()) // 有终止时间
        {
            int minute = caps.at(5).toInt();
            int second = caps.at(6).toInt();
            int ms10 = caps.at(7).toInt();
            lyric.end = minute * 60000 + second*1000 + ms10 * ms10x;
        }
        lyric.text = caps.at(8);
        lyricStream.append(lyric);
        currentTime = lyric.start;
    }

    currentRow = 0;
    update();
}

void DesktopLyricWidget::setColors(QColor p, QColor w)
{
    playingColor = p;
    waitingColor = w;
    update();
}

void DesktopLyricWidget::showEvent(QShowEvent *event)
{
     restoreGeometry(settings.value("music/desktopLyricGeometry").toByteArray());
}

void DesktopLyricWidget::hideEvent(QHideEvent *event)
{
     settings.setValue("music/desktopLyricGeometry", this->saveGeometry());
}

bool DesktopLyricWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)
    MSG* msg = static_cast<MSG*>(message);
    switch(msg->message)
    {
    case WM_NCHITTEST:
        const auto ratio = devicePixelRatioF(); // 解决4K下的问题
        int xPos = static_cast<int>(GET_X_LPARAM(msg->lParam) / ratio - this->frameGeometry().x());
        int yPos = static_cast<int>(GET_Y_LPARAM(msg->lParam) / ratio - this->frameGeometry().y());
        if(xPos < boundaryWidth &&yPos<boundaryWidth)                    //左上角
            *result = HTTOPLEFT;
        else if(xPos>=width()-boundaryWidth&&yPos<boundaryWidth)          //右上角
            *result = HTTOPRIGHT;
        else if(xPos<boundaryWidth&&yPos>=height()-boundaryWidth)         //左下角
            *result = HTBOTTOMLEFT;
        else if(xPos>=width()-boundaryWidth&&yPos>=height()-boundaryWidth)//右下角
            *result = HTBOTTOMRIGHT;
        else if(xPos < boundaryWidth)                                     //左边
            *result =  HTLEFT;
        else if(xPos>=width()-boundaryWidth)                              //右边
            *result = HTRIGHT;
        /*else if(yPos<boundaryWidth)                                       //上边
            *result = HTTOP;*/
        else if(yPos>=height()-boundaryWidth)                             //下边
            *result = HTBOTTOM;
        else              //其他部分不做处理，返回false，留给其他事件处理器处理
           return false;
        return true;
    }
    return false;         //此处返回false，留给其他事件处理器处理
}

void DesktopLyricWidget::enterEvent(QEvent *event)
{
    hovering = true;
    update();
}

void DesktopLyricWidget::leaveEvent(QEvent *event)
{
    if (!this->geometry().contains(QCursor::pos()))
    {
        hovering = false;
    }
    else
    {
        QTimer::singleShot(300, [=]{
            if (!this->geometry().contains(QCursor::pos()))
            {
                hovering = false;
                update();
            }
        });
    }
    update();
}

void DesktopLyricWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        pressPos = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void DesktopLyricWidget::mouseMoveEvent(QMouseEvent *event)
{
    hovering = true;
    if(event->buttons()&Qt::LeftButton)
    {
        move(QCursor::pos() - pressPos);
    }
    QWidget::mouseMoveEvent(event);
}

void DesktopLyricWidget::resizeEvent(QResizeEvent *event)
{

}

void DesktopLyricWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 绘制背景
    if (jiWindow)
    {
        painter.fillRect(this->rect(), bgColor);
    }
    if (hovering)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addRoundedRect(this->rect(), 5, 5);
        painter.fillPath(path, QColor(64, 64, 64, 32));
    }

    // 绘制桌面歌词
    QFont font;
    font.setPointSize(pointSize);
    font.setBold(true);

    font.setPointSize(pointSize);
    QFontMetrics fm(this->font());
    fontHeight = fm.height();
    lineSpacing = fm.lineSpacing();
    painter.setFont(font);

    // 文字区域
    QRect rect = this->rect();
    rect.setLeft(rect.left() + boundaryWidth);
    rect.setTop(rect.top() + boundaryWidth);
    rect.setRight(rect.right() - boundaryWidth);
    rect.setBottom(rect.bottom() - boundaryWidth);

    if (currentRow > -1 && currentRow < lyricStream.size())
    {
        bool cross = lineMode != SingleLine && currentRow % 2;

        // 绘制当前句
        if (currentRow + (cross ? 1 : 0) < lyricStream.size())
        {
            painter.setPen(cross ? waitingColor : playingColor);
            QFlags<Qt::AlignmentFlag> align;
            if (lineMode == SuitableLine || lineMode == DoubleLine)
            {
                align = Qt::AlignTop;
                this->setMinimumHeight((fontHeight + boundaryWidth) * 4);
                this->setMinimumWidth((fm.horizontalAdvance("一二三四五六七八九十十一十二")) * 2);
            }
            else if (lineMode == SingleLine)
            {
                align = Qt::AlignVCenter;
                this->setMinimumHeight((fontHeight + boundaryWidth) * 2);
                this->setMinimumWidth((fm.horizontalAdvance("一二三四五六七八九十十一十二")) * 2);
            }
            if (alignMode == AlignMid)
                align |= Qt::AlignHCenter;
            else if (alignMode == AlignRight)
                align |= Qt::AlignRight;
            else
                align |= Qt::AlignLeft;
            painter.drawText(rect, align, lyricStream.at(currentRow + (cross ? 1 : 0)).text);
        }

        // 绘制下一句
        if (currentRow - (cross ? 1 : 0) < lyricStream.size()-1 && (lineMode == SuitableLine || lineMode == DoubleLine))
        {
            painter.setPen(cross ? playingColor : waitingColor);
            QFlags<Qt::AlignmentFlag> align = Qt::AlignBottom;
            if (alignMode == AlignMid)
                align |= Qt::AlignHCenter;
            else if (alignMode == AlignLeft)
                align |= Qt::AlignLeft;
            else
                align |= Qt::AlignRight;

            painter.drawText(rect, align, lyricStream.at(currentRow + (cross ? 0 : 1)).text);
        }
    }
}

void DesktopLyricWidget::showMenu()
{
    QMenu* menu = new QMenu(this);
    QMenu *lineMenu = new QMenu("行数", menu);
    QMenu *alignMenu = new QMenu("对齐", menu);
    QMenu *fontMenu = new QMenu("字体", menu);
    QAction *playingC = new QAction("已播放颜色", this);
    QAction *waitingC = new QAction("未播放颜色", this);
    QAction *backgroundC = new QAction("背景颜色", this);
    QAction *transMode = new QAction("透明模式", this);
    QAction *hide = new QAction("隐藏", this);

    menu->addAction(playingC);
    menu->addAction(waitingC);
    menu->addAction(backgroundC);
    menu->addAction(transMode);
    menu->addMenu(lineMenu);
    menu->addMenu(alignMenu);
    menu->addMenu(fontMenu);
    menu->addAction(hide);

    connect(playingC, &QAction::triggered, [=]{
        QColor c  = QColorDialog::getColor(playingColor, this, "选择已播放颜色", QColorDialog::ShowAlphaChannel);
        if (!c.isValid())
            return ;
        if (c != playingColor)
        {
            settings.setValue("lyric/playingColor", playingColor = c);
            update();
        }
    });

    connect(waitingC, &QAction::triggered, [=]{
        QColor c = QColorDialog::getColor(waitingColor, this, "选择未播放颜色", QColorDialog::ShowAlphaChannel);
        if (!c.isValid())
            return ;
        if (c != waitingColor)
        {
            settings.setValue("lyric/waitingColor", waitingColor = c);
            update();
        }
    });

    connect(backgroundC, &QAction::triggered, [=]{
        if (!jiWindow)
            backgroundC->setEnabled(false);
        QColor c = QColorDialog::getColor(bgColor, this, "选择背景颜色", QColorDialog::ShowAlphaChannel);
        if (!c.isValid())
            return ;
        if (c != bgColor)
        {
            settings.setValue("lyric/bgColor", waitingColor = c);
            update();
        }
    });

    connect(transMode, &QAction::triggered, [=]{
        if (!jiWindow)
        transMode->setCheckable(true);
        transMode->setChecked(true);
        bool trans = settings.value("music/desktopLyricTrans", false).toBool();
        settings.setValue("music/desktopLyricTrans", !trans);
        emit signalSWitchTrans();
    });

    QStringList modeline{"自动", "单行", "双行"};
    for (int index = 0; index < modeline.size(); index++)
    {
        QAction* lm = new QAction(modeline.at(index), this);
        lineMenu->addAction(lm);
        if (lineMode == index)
        {
            lm->setCheckable(true);
            lm->setChecked(true);
        }
        connect(lm, &QAction::triggered, [=]{
            lineMode = static_cast<LineMode>(index);
            settings.setValue("music/lineMode", lineMode);
            update();
        });
    }

    QStringList modealign{"居中", "左右分离", "左对齐", "右对齐"};
    for (int index = 0; index < modealign.size(); index++)
    {
        QAction* am = new QAction(modealign.at(index), this);
        alignMenu->addAction(am);
        if (alignMode == index)
        {
            am->setCheckable(true);
            am->setChecked(true);
        }
        connect(am, &QAction::triggered, [=]{
           alignMode = static_cast<AlignMode>(index);
           settings.setValue("music/alignMode", alignMode);
           update();
        });
    }

    for (int i = 10; i < 40; i++)
    {
         QAction* fm = new QAction(QString::number(i), this);
         fontMenu->addAction(fm);
         if(pointSize == i)
         {
             fm->setCheckable(true);
             fm->setChecked(true);
         }
         connect(fm, &QAction::triggered, [=]{
             pointSize = i;
             settings.setValue("lyric/pointSize", pointSize);
             update();
         });
    }
    connect(hide, &QAction::triggered, this, [=]{
        this->hide();
        emit signalhide();
    });

    menu->exec(cursor().pos());

    QList<QAction*> list = menu->actions();
    foreach(QAction* action, list)
        delete action;
    delete menu;
}

void DesktopLyricWidget::setPosition(qint64 position)
{
    if (!lyricStream.size())
        return ;
    if (currentRow < 0 || currentRow >= lyricStream.size())
        currentRow = 0;

    LyricBean lyric = lyricStream.at(currentRow);
    if (currentRow == lyricStream.size()-1 && lyric.start <= position) // 已经到末尾了
        return ;

    LyricBean nextLyric = currentRow == lyricStream.size()-1 ? LyricBean(false) : lyricStream.at(currentRow + 1);
    if (lyric.start <= position && nextLyric.start >= position) // 不需要改变
        return ;

    if (lyric.start > position) // 什么情况？从头强制重新开始！
        currentRow = 0;

    while (currentRow+1 < lyricStream.size() && lyricStream.at(currentRow+1).start < position)
    {
        currentRow++;
    }
    update();
}
