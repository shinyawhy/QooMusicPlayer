#ifndef LYRICSTREAMWIDGET_H
#define LYRICSTREAMWIDGET_H

#include <QWidget>
#include <QPainter>
#include "desktoplyricwidget.h"

class LyricStreamWidget : public QWidget
{
    Q_OBJECT
public:
    LyricStreamWidget(QWidget* parent = nullptr) : QWidget(parent), settings(QApplication::applicationDirPath()+"/musics.ini", QSettings::Format::IniFormat),
        updateTimer(new QTimer(this))
    {
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu()));
        playingColor = qvariant_cast<QColor>(settings.value("lyric/playingColor", playingColor));
        waitingColor = qvariant_cast<QColor>(settings.value("lyric/waitingColor", waitingColor));
        pointSize = settings.value("lryic/pointSize", pointSize).toInt();

        connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
        updateTimer->setInterval(16);
    }

    void updateFixedHeight()
    {
        QFont font = this->font();
        font.setPointSize(pointSize);
        QFontMetrics fm(font);
        this->lineSpacing = fm.height() + (fm.lineSpacing() - fm.height()) * 2;
        setFixedHeight((lyricStream.size() + verticalMargin * 2) * lineSpacing);
    }

    void setLyric(QString text)
    {
        // 检测是不是全是毫秒还是10毫秒
        int ms10x = 10;
        QRegularExpression re10("\\[\\d{2}:\\d{2}\\.([1-9]\\d{2})\\]");
        QRegularExpressionMatch match10;
        if (text.lastIndexOf(re10, -1, &match10) != -1)
        {
            int val = match10.captured(1).toInt();
            qDebug() << " 匹配项：" << val;
            if (val > 0) // 存在不为0的三位数
            {
                qDebug() << "检测到勉强受支持的歌词格式：第三位是毫秒";
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
        updateFixedHeight();
        update();
    }

    /**
     * @return 当前行有没有改变
     */
    bool setPosition(qint64 position)
    {
        int prevRow = currentRow;
        if (!lyricStream.size())
            return false;
        if (currentRow < 0 || currentRow >= lyricStream.size())
            currentRow = 0;

        LyricBean lyric = lyricStream.at(currentRow);
        if (currentRow == lyricStream.size()-1 && lyric.start <= position) // 已经到末尾了
            return false;

        LyricBean nextLyric = currentRow == lyricStream.size()-1 ? LyricBean(false) : lyricStream.at(currentRow + 1);
        if (lyric.start <= position && nextLyric.start >= position) // 不需要改变
            return false;

        if (lyric.start > position) // 什么情况？从头强制重新开始！
            currentRow = 0;

        while (currentRow+1 < lyricStream.size() && lyricStream.at(currentRow+1).start < position)
        {
            currentRow++;
        }
        if (currentRow == prevRow)
            return false;
        switchRowTimestamp = QDateTime::currentMSecsSinceEpoch();
        updateTimer->start();
        update();
        return true;
    }

    int getCurrentTop() const
    {
        return (currentRow + verticalMargin + 0.5) * lineSpacing;
    }

    void setColors(QColor p, QColor w)
    {
        playingColor = p;
        waitingColor = w;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        QFont font(painter.font());
        font.setPointSize(pointSize);
        painter.setFont(font);
        painter.setPen(waitingColor);
        QFontMetrics fm(font);
        int lineSpacing = fm.height() + (fm.lineSpacing() - fm.height()) * 2;
        double rowOffset = verticalMargin;
        qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
        qint64 aniDelta = currentTimestamp - switchRowTimestamp;
        bool animating = aniDelta < switchRowDuration;
        const double scale = 0.5;
        for (int i = 0; i < lyricStream.size(); i++)
        {
            double row = i + rowOffset;
            int top = row * lineSpacing;
            if (i == currentRow - 1 && animating)
            {
                painter.save();
                double prop = scale - scale * aniDelta / switchRowDuration;
                QFont font(painter.font());
                font.setPointSize(pointSize * (1+prop));
                painter.setFont(font);
                QRect lineRect(0, top - lineSpacing*prop/2, width(), lineSpacing * (1 + prop));
                painter.drawText(lineRect, Qt::AlignCenter, lyricStream.at(i).text);
                painter.restore();
                rowOffset += prop;
            }
            else if (i == currentRow)
            {
                painter.save();
                if (animating)
                {
                    double prop = scale * aniDelta / switchRowDuration;
                    QRect lineRect(0, top - lineSpacing*prop/2, width(), lineSpacing * (1 + 2 * prop));
                    QFont font(painter.font());
                    font.setPointSize(pointSize * (1+prop));
                    painter.setPen(playingColor);
                    painter.setFont(font);
                    painter.drawText(lineRect, Qt::AlignCenter, lyricStream.at(i).text);
                    rowOffset += prop;
                }
                else
                {
                    QRect lineRect(0, top - lineSpacing*0.25, width(), lineSpacing * (1+scale*2));
                    QFont font(painter.font());
                    font.setPointSize(pointSize * (1+scale));
                    painter.setPen(playingColor);
                    painter.setFont(font);
                    painter.drawText(lineRect, Qt::AlignCenter, lyricStream.at(i).text);
                    rowOffset += scale;

                    if (updateTimer->isActive())
                        updateTimer->stop();
                }
                painter.restore();
            }
            else
            {
                QRect lineRect(0, top, width(), lineSpacing);
                painter.drawText(lineRect, Qt::AlignCenter, lyricStream.at(i).text);
            }
        }

    }

    void adjustLyricTime(int begin, int offset)
    {
        if (!lyricStream.size())
            return ;

        int ms10 = 10;
        foreach (LyricBean lyric, lyricStream)
        {
            if (lyric.start % 10 != 0)
            {
                ms10 = 1;
                qDebug() << "第三位使用毫秒" << lyric.start << lyric.text;
                break;
            }
        }

        QStringList sl;
        for (int i = 0; i < lyricStream.size(); i++)
        {
            LyricBean line = lyricStream.at(i);
            if (i >= begin && line.start != 0)
            {
                line.start += offset;
                if (line.end)
                    line.end += offset;
                if (line.start < 0)
                    line.start = 0;
                if (line.end < 0)
                    line.end = 0;
                lyricStream[i] = line;
            }

            // 歌词转换回字符串
            int minute = line.start / 60000;
            int second = line.start % 60000 / 1000;
            int msecond = line.start % 1000 / ms10;
            QString s = QString("[%1:%2.%3]")
                    .arg(minute, 2, 10, QLatin1Char('0'))
                    .arg(second, 2, 10, QLatin1Char('0'))
                    .arg(msecond, ms10 == 1 ? 3 : 2, 10, QLatin1Char('0'));
            if (line.end)
            {
                int minute = line.end / 60000;
                int second = line.end % 60000 / 1000;
                int msecond = line.end % 1000 / ms10;
                s += QString("[%1:%2.%3]")
                        .arg(minute, 2, 10, QLatin1Char('0'))
                        .arg(second, 2, 10, QLatin1Char('0'))
                        .arg(msecond, ms10 == 1 ? 3 : 2, 10, QLatin1Char('0'));
            }
            s += line.text;
            sl << s;
        }
        emit signalAdjustLyricTime(sl.join("\n"));
    }

signals:
    void signalSwitchCoverBlur();
    void signalAdjustLyricTime(QString);

private slots:
    void showMenu()
    {
       QMenu* menu = new QMenu(this);
       QAction *playingC = new QAction("选择已播放颜色",this);
       QAction *waitingC = new QAction("选择未播放颜色", this);
       QMenu *fontMenu = new QMenu("字体大小", menu);
       QMenu *timeMenu = new QMenu("调整速度", menu);

       menu->addAction(playingC);
       menu->addAction(waitingC);
       menu->addMenu(fontMenu);
       menu->addMenu(timeMenu);

       connect(playingC, &QAction::triggered, [=]{
           QColor c = QColorDialog::getColor(playingColor, this, "选择已播放颜色", QColorDialog::ShowAlphaChannel);
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
       for (int i = 8; i < 30; i++)
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
                updateFixedHeight();
                update();
            });


       }

       timeMenu->addAction("加快5秒",[=]{adjustLyricTime(0, -5000);});
       timeMenu->addAction("加快2秒",[=]{adjustLyricTime(0, -2000);});
       timeMenu->addAction("加快1秒",[=]{adjustLyricTime(0, -1000);});
       timeMenu->addAction("减慢1秒",[=]{adjustLyricTime(0, +1000);});
       timeMenu->addAction("减慢2秒",[=]{adjustLyricTime(0, +2000);});
       timeMenu->addAction("减慢5秒",[=]{adjustLyricTime(0, +5000);});

       menu->exec(cursor().pos());

       QList<QAction*> list = menu->actions();
       foreach(QAction* action, list)
           delete action;
       delete menu;
    }

private:
    QSettings settings;
    LyricStream lyricStream;
    int currentRow = -1;
    qint64 switchRowTimestamp = 0;
    const int switchRowDuration = 200;
    QTimer* updateTimer;
    int lineSpacing =30;
    int verticalMargin = 5; // 上下间距5行

    int pointSize = 12;
    QColor playingColor = QColor(0, 255, 255);
    QColor waitingColor = Qt::white;
};

#endif // LYRICSTREAMWIDGET_H
