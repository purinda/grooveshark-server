#ifndef QPLAYER_H
#define QPLAYER_H

#include "QtCore"
#include <QMediaPlayer>
#include <QObject>
#include "qrequest.h"
#include "qgroovesession.h"

namespace Player {
    enum Command {
        Play,
        Pause,
        Stop,
        SetVol
    };
}

class QPlayer : public QObject
{
private:
    Q_OBJECT

    QGrooveSession *gsSession;
    QRequest *gsRequest;
    QMediaPlayer *player;
    int volumeLevel = 100;

    QMap<QString, QVariant> getSongData(QString reponse);

private slots:
    void onTokenDataLoaded();
    void onResponse(int postActionId, QString response);

public:
    explicit QPlayer(QObject *parent = 0);

    void getStreamKeyFromSongIDEx(ulong songId);

public slots:
    void start();

    // Player control
    void onCommand(Player::Command command, quint32 param1);
    void onBufferingProgress(int progress);
};

#endif // QPLAYER_H

