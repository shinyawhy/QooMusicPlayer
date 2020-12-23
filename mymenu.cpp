#include "mymenu.h"

MyMenu::MyMenu(QObject *parent)
{

}

MyMenu::~MyMenu()
{

}

void MyMenu::mouseReleaseEvent(QMouseEvent *event)
{
   QAction *action = this->actionAt(event->pos());
   if (action)
   {
       action->activate(QAction::Trigger);
   }
   else
   {
       QMenu::mouseReleaseEvent(event);
   }
}

bool MyMenu::event(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::ToolTip:
    //show action tooltip instead of widget tooltip
    {
    QHelpEvent* he = dynamic_cast<QHelpEvent*>(e);
    QAction* act = actionAt(he->pos());
    if( act )
    {
    QToolTip::showText(he->globalPos(), act->toolTip(), this);
    return true;
    }
    }
    break;
    }
    QMenu::event(e);
}
