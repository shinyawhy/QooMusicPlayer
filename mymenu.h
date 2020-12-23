#ifndef MYMENU_H
#define MYMENU_H
#include <QMenu>
#include <QObject>
#include <QMouseEvent>
#include <QToolTip>

class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QObject *parent = 0);
    ~MyMenu() override;

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool event(QEvent *e) override;
};

#endif // MYMENU_H
