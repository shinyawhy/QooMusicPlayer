#ifndef MYMENU_H
#define MYMENU_H
#include<QMenu>
#include<QObject>
#include <QMouseEvent>

class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QObject *parent = 0);
    ~MyMenu() override;

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // MYMENU_H
