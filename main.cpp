#include <QCoreApplication>
#include <QThread>
#include <QObject>
#include <QtCore>

#include "qserver.h"
#include "lib/grooveshark/qplayer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //
    // Start TCP Server thread
    //
    QServer *tcpServer    = new QServer();
    QThread *threadServer = new QThread;
    tcpServer->moveToThread(threadServer);

    QObject::connect(threadServer,SIGNAL(started()),
                     tcpServer, SLOT(start()));
    QObject::connect(tcpServer, SIGNAL(finished()),
                     threadServer, SLOT(quit()));
    QObject::connect(tcpServer, SIGNAL(finished()),
                     tcpServer, SLOT(deleteLater()));
    QObject::connect(threadServer, SIGNAL(finished()),
                     threadServer, SLOT(deleteLater()));

    threadServer->start();

    //
    // Start Grooveshark Player thread
    //
    QPlayer *gsPlayer     = new QPlayer();
    QThread *threadPlayer = new QThread;
    gsPlayer->moveToThread(threadServer);

    QObject::connect(threadPlayer,SIGNAL(started()),
                     gsPlayer, SLOT(start()));
    QObject::connect(threadPlayer, SIGNAL(finished()),
                     threadPlayer, SLOT(deleteLater()));

    threadPlayer->start();

    return a.exec();
}
