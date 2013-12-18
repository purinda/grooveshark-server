#include "qserver.h"

QServer::QServer(QObject *parent) :
    QTcpServer(parent) {
}

void QServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "Incoming connection... " << endl;

    QServerChildThread *clientConnectionThread = new QServerChildThread(socketDescriptor, this);

    QObject::connect(clientConnectionThread, SIGNAL(playSong(quint32)),
                     this, SLOT(playSong(quint32)));

    QObject::connect(clientConnectionThread, SIGNAL(pauseSong()),
                     this, SLOT(pauseSong()));

    QObject::connect(clientConnectionThread, SIGNAL(setVolume(int)),
                     this, SLOT(setVolume(int)));

    QObject::connect(clientConnectionThread, SIGNAL(stopSong()),
                     this, SLOT(stopSong()));

    QObject::connect(clientConnectionThread, SIGNAL(error(QTcpSocket::SocketError)),
                     this, SLOT(onClientSocketError(QTcpSocket::SocketError)));

    QObject::connect(clientConnectionThread, SIGNAL(finished()),
                     clientConnectionThread, SLOT(deleteLater()));

    clientConnectionThread->start();
}

void QServer::playSong(quint32 songId) {
    emit sendCommand(Player::Play, songId);
}

void QServer::pauseSong() {
    emit sendCommand(Player::Pause);
}

void QServer::stopSong() {
    emit sendCommand(Player::Stop);
}

void QServer::setVolume(int volume) {
    emit sendCommand(Player::SetVol, volume);
}

// Handle socket errors or just print them out for now.
void QServer::onClientSocketError(QTcpSocket::SocketError error) {
    qDebug() << "Client socket error: " << error << endl;
}
