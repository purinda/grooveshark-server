#include "qserver.h"

QServer::QServer(QObject *parent) :
    QObject(parent) {

}

void QServer::handleRequest() {
    qDebug() << "Client connected" << endl;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);

    out << (quint16)0;
    out << "Hello from server" << endl;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));


    QString line;
    do {
        line = readLine( clientConnection );
        if (line.length() > 0) {
            writeLine( clientConnection, line );
        }
    } while (line.length() > 0);

    clientConnection->disconnectFromHost();
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

void QServer::start() {

    if (networkSession) {
         QNetworkConfiguration config = networkSession->configuration();
         QString id;
         if (config.type() == QNetworkConfiguration::UserChoice)
             id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
         else
             id = config.identifier();

         QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
         settings.beginGroup(QLatin1String("QtNetwork"));
         settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
         settings.endGroup();
     }

     tcpServer = new QTcpServer(this);
     if (!tcpServer->listen()) {
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

     qDebug() << (tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                     "Send Grooveshark SongIDs to the listening service.")
                     .arg(ipAddress).arg(tcpServer->serverPort()));

     connect(this->tcpServer, SIGNAL(newConnection()), this, SLOT(handleRequest()));

}
