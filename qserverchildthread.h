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
    explicit QServerChildThread(QObject *parent = 0);

    QServerChildThread(int socketDescriptor, QObject *parent);
    void run();

private:
    int socketDescriptor;

signals:
    void error(QTcpSocket::SocketError socketError);

public slots:

};

#endif // QSERVERCHILDTHREAD_H
