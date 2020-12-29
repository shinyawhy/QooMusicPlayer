#ifndef MUSIC_H
#define MUSIC_H
#include <QUrl>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QDateTime>
#include <QSettings>

#define JSON_VAL_LONG(json, x) static_cast<qint64>(json.value(#x).toDouble())
#define JVAL_LONG(x) static_cast<qint64>(json.value(#x).toDouble())
#define JVAL_INT(x) json.value(#x).toInt()
#define JVAL_STR(x) json.value(#x).toString()
#define snum(x) QString::number(x)

struct Artist
{
    qint64 id = 0;
    QString name;
    QString faceUrl;

    static Artist fromJson(QJsonObject json)
    {
        Artist artist;
        artist.id = JVAL_LONG(id);
        artist.name = JVAL_STR(name);
        artist.faceUrl = JVAL_STR(img1v1Url);
        return artist;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("faceUrl", faceUrl);
        return json;
    }
};

struct Album
{
    qint64 id = 0;
    QString name;
    int size = 0;
    int mark = 0;

    static Album fromJson(QJsonObject json)
    {
        Album album;
        album.id = JVAL_LONG(id);
        album.name = JVAL_STR(name);
        album.size = JVAL_INT(size);
        return album;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("size", size);
        return json;
    }
};
struct Music
{
    qint64 id = 0;
    QString name;
    int duration = 0;
    QList<Artist> artists;
    Album album;
    QString artistNames;

    static Music fromJson(QJsonObject json)
    {
        Music music;
        music.id = JVAL_LONG(id);
        music.name = JVAL_STR(name);
        QJsonArray array = json.value("artists").toArray(); // json中读出的artist放入一个数组
        QStringList artistNameList;
        foreach (QJsonValue val, array)
        {
           Artist artist = Artist::fromJson(val.toObject());
           music.artists.append(artist);
           artistNameList.append(artist.name);
        }
        
        music.artistNames = artistNameList.join("/");
        music.album = Album::fromJson(json.value("album").toObject());
        music.duration = JVAL_INT(duration);
        return music;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("duration", duration);
        QJsonArray array;
        foreach (Artist artist, artists)
            array.append(artist.toJson());
        json.insert("artists", array);
        json.insert("album", album.toJson());
        return json;
    }

    bool isValid() const
    {
        return id;
    }

    bool operator==(const Music& music)
    {
        return this->id == music.id;
    }

    QString simpleString() const
    {
        return name + "-" + artistNames;
    }

};

struct PlayListCreator
{
    QString nickname;
    QString signature;
    QString avatarUrl;
};

struct PlayList
{
  QString name;
  qint64 id;
  QString description;
  QStringList tags;
  int playCount;
  PlayListCreator creator;

  QList<Music> contiansMusic;

  static PlayList fromJson(QJsonObject json)
  {
      PlayList playlist;
//      music.id = JVAL_LONG(id);
      playlist.name = JVAL_STR(name);

      QJsonArray array = json.value("contiansMusic").toArray();
      QStringList contiansMusicList;
      foreach (QJsonValue val, array)
      {
          Music music = Music::fromJson(val.toObject());
          playlist.contiansMusic.append(music);
          contiansMusicList.append(music.name);
      }
//      music.artistNames = artistNameList.join("/");
//      music.album = Album::fromJson(json.value("album").toObject());
//      music.duration = JVAL_INT(duration);
      return playlist;
  }

  QJsonObject toJson() const
  {
      QJsonObject json;
//      json.insert("id", id);
      json.insert("name", name);
      QJsonArray array;
      foreach (Music music, contiansMusic)
          array.append(music.toJson());
      json.insert("contiansMusic", array);
//      json.insert("duration", duration);
//      json.insert("album", album.toJson());
      return json;
  }

  bool operator==(const PlayList& pl)
  {
      return this->id == pl.id;
  }
};

typedef QList<Music> SongList;
typedef QList<PlayList> PlayListList;

#endif // MUSIC_H
