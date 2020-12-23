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
