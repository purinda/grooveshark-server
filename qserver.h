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
#include "qconfig.h"

class QTcpServer;
class QNetworkSession;

class QServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *tcpServer;
    QNetworkSession *networkSession;
    QTcpSocket *clientConnection;

    QMutex mMutex, mClientConnection, mRequestLocker;
    bool mRunThread;

    bool getRunThread();
    int waitForInput(QTcpSocket *socket);
    QString readLine(QTcpSocket *socket);
    void writeLine(QTcpSocket *socket, const QString &line);

private slots:
    void onClientRequest();
    void onResponse();

public:
    explicit QServer(QObject *parent = 0);

public slots:
    void start();

signals:
    void finished();

    // Signals used for controlling the player
    void playSong(ulong songId);
    void setVolume(int volume);
    void pauseSong();
    void playSong();
    void stopSong();
};

#endif // QSERVER_H
