#ifndef QSERVER_H
#define QSERVER_H

#include <QObject>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>
#include <QDataStream>
#include <QMutex>
#include <QThread>
#include <QRegExp>
#include "qserverchildthread.h"
#include "lib/grooveshark/qplayer.h"
#include "qconfig.h"

class QServer : public QTcpServer
{
    Q_OBJECT
private:
    QNetworkSession *networkSession;

    QMutex mMutex, mClientConnection, mRequestLocker;
    bool mRunThread;

protected:
    void incomingConnection(qintptr socketDescriptor);

public:
    QServer(QObject *parent = 0);

public slots:
    void playSong(quint32);
    void pauseSong();
    void stopSong();
    void setVolume(int volume);

    // Handle client socket errors
    void onClientSocketError(QTcpSocket::SocketError error);

signals:
    void finished();
    void sendCommand(Player::Command command, quint32 param1 = 0);

};

#endif // QSERVER_H
