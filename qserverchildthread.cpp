#include "qserverchildthread.h"
#include <QIODevice>

QServerChildThread::QServerChildThread(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor) {
    qDebug() << "Client connection thread initialised... " << endl;
}


void QServerChildThread::run() {
    qDebug() << "TCP Client Thread Running..." << endl;

    this->clientConnection = new QTcpSocket();

    if (!this->clientConnection->setSocketDescriptor(this->socketDescriptor, QTcpSocket::ConnectedState, QIODevice::ReadWrite)) {
        emit error(this->clientConnection->error());
        return;
    }

    this->clientConnection->open(QIODevice::ReadWrite);

    QObject::connect(this->clientConnection, SIGNAL(readyRead()),
                     this, SLOT(onResponse()), Qt::ConnectionType::DirectConnection);

    // Keep the thread running (fucking important!)
    this->exec();
}


void QServerChildThread::onResponse() {
    qDebug() << "Request received " << endl;

    QRegExp matchRequests("(\\w+)\\s?(\\d*)");
    QString command, param;

    qDebug() << "Bytes available: " << clientConnection->bytesAvailable() << endl;
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
                emit playSong(param.toInt());
            } else {
                // Continue playing a paused/stopped song
                emit playSong(0);
            }
        }

        if (command == "PAUSE") {
            emit pauseSong();
        }

        if (command == "STOP") {
            emit stopSong();
        }

        if (command == "VOL") {
            if (param.toInt() >= 0 && param.toInt() <= 100) {
                emit setVolume(param.toInt());
            }
        }

        if (command == "SAYBYE") {
            clientConnection->disconnectFromHost();
        }

        qDebug() << ">> " << line << endl;
    }

}

QString QServerChildThread::readLine(QTcpSocket *socket )
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

void QServerChildThread::writeLine(QTcpSocket *socket, const QString &line) {
    if (line.length() > 0) {
        socket->write(line.toLocal8Bit().data());

        if (! socket->waitForBytesWritten()) {
            printf("writeLine: the write to the socket failed\n");
        }
    }
}

int QServerChildThread::waitForInput( QTcpSocket *socket ) {
    int bytesAvail = -1;
    while (socket->state() == QAbstractSocket::ConnectedState && bytesAvail < 0) {
        if (socket->waitForReadyRead( 100 )) {
            bytesAvail = socket->bytesAvailable();
        }
        else {
            QThread::msleep(50);
        }
    }
    return bytesAvail;
}

