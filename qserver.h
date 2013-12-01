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

class QTcpServer;
class QNetworkSession;

class QServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer *tcpServer;
    QNetworkSession *networkSession;
    QMutex mMutex;
    bool mRunThread;

    bool getRunThread();
    int waitForInput(QTcpSocket *socket);
    QString readLine(QTcpSocket *socket);
    void writeLine(QTcpSocket *socket, const QString &line);


private slots:
    void handleRequest();

public:
    explicit QServer(QObject *parent = 0);

public slots:
    void start();

signals:
    void finished();
};

#endif // QSERVER_H
