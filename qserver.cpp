#include "qserver.h"

QServer::QServer(QObject *parent) :
    QObject(parent) {

}

void QServer::start() {
     tcpServer = new QTcpServer(this);

     if (!tcpServer->listen(QHostAddress::Any, QConfig::instance()->getSetting("Grooveshark/port", 16444).toInt())) {
         qDebug() << "Unable to start the GrooveShark server! Aborting... " << endl;
         tcpServer->close();
         return;
     }

     QString ipAddress;
     QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
     // use the first non-localhost IPv4 address
     for (int i = 0; i < ipAddressesList.size(); ++i) {
         if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
             ipAddressesList.at(i).toIPv4Address()) {
             ipAddress = ipAddressesList.at(i).toString();
             break;
         }
     }
     // if we did not find one, use IPv4 localhost
     if (ipAddress.isEmpty()) {
         ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
     }

     qDebug() << (tr("Grooveshark Server Started\n"
                     "The server is running on\n\nIP: %1\nport: %2\n\n")
                     .arg(ipAddress).arg(tcpServer->serverPort()));

     connect(this->tcpServer, SIGNAL(newConnection()),
             this, SLOT(onClientRequest()), Qt::QueuedConnection);

}

void QServer::onClientRequest() {
    QMutexLocker locker(&mClientConnection);

    qDebug() << "Client connected" << endl;

    clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    connect(clientConnection, SIGNAL(readyRead()),
            this, SLOT(onResponse()), Qt::QueuedConnection);
}

void QServer::onResponse() {
    QMutexLocker locker(&mRequestLocker);

    qDebug() << "Request received " << endl;

    QRegExp matchRequests("(\\w+)\\s?(\\d*)");
    QString command, param;

    while(clientConnection->canReadLine())
    {
        QByteArray ba = clientConnection->readLine();
        QString line  = QString(ba).simplified();
        qDebug() << "Request: " << line << endl;
        command.clear();
        param.clear();

        if (matchRequests.indexIn(line) != -1) {
            command = matchRequests.cap(1);
            param   = matchRequests.cap(2);
        }

        if (command == "PLAY") {
            if (param.toInt() != 0) {
                // Start a new song
                emit playSong(param.toULong());
            } else {
                // Continue playing a paused/stopped song
                emit playSong();
            }
        }

        if (command == "PAUSE") {
            emit pauseSong();
        }

        if (command == "STOP") {
            emit stopSong();
        }

        if (command == "VOL") {
            if (param.toInt() != 0) {
                emit setVolume(param.toInt());
            }
        }

        if (command == "SAYBYE") {
            clientConnection->disconnectFromHost();
        }

        qDebug() << ">> " << line << endl;
    }

}

QString QServer::readLine(QTcpSocket *socket )
{
    QString line = "";
    int bytesAvail = waitForInput( socket );
    if (bytesAvail > 0) {
        int cnt = 0;
        bool endOfLine = false;
        bool endOfStream = false;
        while (cnt < bytesAvail && (!endOfLine) && (!endOfStream)) {
            char ch;
            int bytesRead = socket->read(&ch, sizeof(ch));
            if (bytesRead == sizeof(ch)) {
                cnt++;
                line.append( ch );
                if (ch == '\r') {
                    endOfLine = true;
                }
            }
            else {
                endOfStream = true;
            }
        }
    }
    return line;
}

void QServer::writeLine(QTcpSocket *socket, const QString &line) {
    if (line.length() > 0) {
        socket->write(line.toLocal8Bit().data());

        if (! socket->waitForBytesWritten()) {
            printf("writeLine: the write to the socket failed\n");
        }
    }
}

int QServer::waitForInput( QTcpSocket *socket ) {
    int bytesAvail = -1;
    while (socket->state() == QAbstractSocket::ConnectedState && getRunThread() && bytesAvail < 0) {
        if (socket->waitForReadyRead( 100 )) {
            bytesAvail = socket->bytesAvailable();
        }
        else {
            QThread::msleep(50);
        }
    }
    return bytesAvail;
}

bool QServer::getRunThread() {
    QMutexLocker lock( &mMutex );
    return mRunThread;
}
