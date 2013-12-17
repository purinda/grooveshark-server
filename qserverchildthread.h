#ifndef QSERVERCHILDTHREAD_H
#define QSERVERCHILDTHREAD_H

#include <QObject>
#include <QtNetwork>
#include <QThread>
#include <QTcpSocket>

class QServerChildThread : public QThread
{
    Q_OBJECT
public:
    QServerChildThread(qintptr socketDescriptor, QObject *parent);
    void run();

private:
    qintptr socketDescriptor;
    QTcpSocket *clientConnection;

    int waitForInput(QTcpSocket *socket);
    QString readLine(QTcpSocket *socket);
    void writeLine(QTcpSocket *socket, const QString &line);

public slots:
    void onResponse();

signals:
    void error(QTcpSocket::SocketError socketError);
    // Signals used for controlling the player
    void playSong(quint32 songId);
    void setVolume(int volume);
    void pauseSong();
    void stopSong();

};

#endif // QSERVERCHILDTHREAD_H
