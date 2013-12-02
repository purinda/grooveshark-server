#include <QCoreApplication>
#include <QThread>
#include <QObject>
#include <QtCore>

#include "qserver.h"
#include "lib/grooveshark/qplayer.h"
#include "qconfig.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Init config
    //QConfig::instance()->initConfig(&a);

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

    //
    // IPC Signals required between server and player
    //
    QObject::connect(tcpServer, SIGNAL(playSong(ulong)),
                     gsPlayer, SLOT(onReceiveSongId(ulong)));
    QObject::connect(tcpServer, SIGNAL(pauseSong()),
                     gsPlayer, SLOT(onPause()));
    QObject::connect(tcpServer, SIGNAL(playSong()),
                     gsPlayer, SLOT(onPlay()));
    QObject::connect(tcpServer, SIGNAL(stopSong()),
                     gsPlayer, SLOT(onStop()));
    QObject::connect(tcpServer, SIGNAL(setVolume(int)),
                     gsPlayer, SLOT(onSetVolume(int)));

    //
    // Start threads
    //
    threadServer->start();
    threadPlayer->start();

    return a.exec();
}
