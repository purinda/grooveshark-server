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
    QConfig::instance()->initConfig(a.applicationDirPath());

    QServer *tcpServer = new QServer();
    if (!tcpServer->listen(QHostAddress::AnyIPv4, 16444)) {
        qDebug() << QString("Unable to start the server: %1.").arg(tcpServer->errorString());
        exit(1);
    }

    QThread *threadServer = new QThread;
    tcpServer->moveToThread(threadServer);

    //
    // Start Grooveshark Player thread
    //
    QPlayer *gsPlayer     = new QPlayer();
    QThread *threadPlayer = new QThread;
    gsPlayer->moveToThread(threadPlayer);

    QObject::connect(threadPlayer,SIGNAL(started()),
                     gsPlayer, SLOT(start()));

    QObject::connect(threadPlayer, SIGNAL(finished()),
                     threadPlayer, SLOT(deleteLater()));

    //
    // IPC Signals required between server and player
    //
    qRegisterMetaType<Player::Command>("Player::Command");
    QObject::connect(tcpServer, SIGNAL(sendCommand(Player::Command, quint32)),
                     gsPlayer, SLOT(onCommand(Player::Command, quint32)),
                     Qt::QueuedConnection);

    //
    // Start threads
    //
    threadPlayer->start();
    threadServer->start();

    const int ret = a.exec();
    QConfig::instance()->writeSettings();

    return ret;
}
