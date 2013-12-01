#ifndef QPLAYER_H
#define QPLAYER_H

#include "QtCore"
#include <QMediaPlayer>
#include <QObject>
#include "qrequest.h"
#include "qgroovesession.h"

class QPlayer : public QObject
{
private:
    Q_OBJECT

    QGrooveSession *gsSession;
    QRequest *gsRequest;
    QMediaPlayer *player;

    QMap<QString, QVariant> getSongData(QString reponse);

private slots:
    void onTokenDataLoaded();
    void onResponse(int postActionId, QString response);

public:
    explicit QPlayer(QObject *parent = 0);

    void getStreamKeyFromSongIDEx(u_int32_t songId);

public slots:
    void start();
};

#endif // QPLAYER_H
