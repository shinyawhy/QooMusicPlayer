#include "musiclist.h"

MusicList::MusicList(const QList<QUrl> &urls, QString iname)
{

}

void MusicList::setName(const QString &iname)
{
    name = iname;
}

QString MusicList::getName()
{
    return name;
}

void MusicList::setSQL(bool on)
{
    sql_flag = on;
}

void MusicList::addMusic(const Music &iMusic)
{

}

Music MusicList::getMusic(int pos)
{

}

void MusicList::removeMusic(int pos)
{

}

void MusicList::showInExplorer(int pos)
{

}

void MusicList::showDetail(int pos)
{

}

void MusicList::remove_all()
{

}

void MusicList::insert_all()
{

}

void MusicList::read_SQL()
{

}

void MusicList::clear()
{

}

void MusicList::addMusic(const QList<QUrl> &urls)
{

}
