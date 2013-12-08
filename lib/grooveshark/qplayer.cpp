#include "qplayer.h"

QPlayer::QPlayer(QObject *parent) : QObject(parent) {

}

// Expected return string:
// stream67a-he.grooveshark.com/stream.php?streamKey=69c53dc42b37b7b467c420e5348245f9c7c29bbe_527cee38_2527ed4_2cfa11c_0_1_80
QMap<QString, QVariant> QPlayer::getSongData(QString response) {

    QJsonDocument jsonDocument = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject parentObject   = jsonDocument.object();
    QJsonValue value           = parentObject.value(QString("result"));

    QMap<QString, QVariant> data;
    data["FileID"]    = value.toObject()["FileID"].toString();
    data["Length"]    = value.toObject()["uSecs"].toString();
    data["StreamKey"] = value.toObject()["streamKey"].toString();
    data["ServerURL"] = value.toObject()["ip"].toString();
    data["SongURL"]   = "http://" + data["ServerURL"].toString() + "/stream.php?streamKey=" + data["StreamKey"].toString();

    return data;
}

void QPlayer::getStreamKeyFromSongIDEx(ulong songId) {
    const QString GS_FUNCTION = "getStreamKeyFromSongIDEx";

    // Generate request parameters JSON string
    QMap<QString, QVariant> parameters;
    parameters["type"]     = 1024;
    parameters["prefetch"] = true;
    parameters["songID"]   = QString::number(songId);
    parameters["country"]  = gsSession->getCountryJsonString();

    // Get header map and generate JSON to complete the request
    QMap<QString, QVariant> request;
    request["parameters"] = parameters;
    request["header"]     = gsSession->getHeaderMap(GS_FUNCTION);
    request["method"]     = GS_FUNCTION;

    QString requestUrl = gsSession->HOST + gsSession->getEndpoint() + "?" + GS_FUNCTION;

    qDebug() << "Request URL: " << requestUrl << endl;
    qDebug() << "Request: " << QRequest::jsonEncode(request) << endl;

    gsRequest->postData(requestUrl, QRequest::jsonEncode(request), 5);
}

void QPlayer::onTokenDataLoaded() {
    // GrooveShark token data ready
    // TODO:
    //  - Set a flag stating that the Grooveshark is ready to accept requests to control the player.
}

void QPlayer::onResponse(int postActionId, QString response) {
    switch (postActionId) {
    case 5:
        QString songURL = getSongData(response)["SongURL"].toString();
        qDebug() << "Buffering... ";

        this->player->setMedia(QUrl(songURL));
        this->player->setVolume(this->volumeLevel);
        this->player->stop();

        break;
    }
}

void QPlayer::onPause() {
    qDebug() << "Song paused" << endl;
    this->player->pause();
}

void QPlayer::onPlay() {
    qDebug() << "Continue playing" << endl;
    this->player->play();
}

void QPlayer::onStop() {
    qDebug() << "Song stopped" << endl;
    this->player->stop();
}

void QPlayer::onSetVolume(int vol) {
    if (vol > 100 || vol < 0) {
        vol = 100;
    }

    this->player->setVolume(vol);
}

void QPlayer::start() {
    this->player    = new QMediaPlayer(0, QMediaPlayer::Flag::StreamPlayback);
    this->gsSession = new QGrooveSession();
    this->gsRequest = new QRequest();

    connect(this->gsSession, SIGNAL(tokenDataLoaded()),
            this, SLOT(onTokenDataLoaded()));

    connect(this->gsRequest, SIGNAL(dataPosted(int, QString)),
            this, SLOT(onResponse(int, QString)));

    connect(this->player, SIGNAL(bufferStatusChanged(int)),
            this, SLOT(onBufferingProgress(int)));
}

void QPlayer::onReceiveSongId(ulong songId) {
    qDebug() << "Request to play song ID: " << songId << " received. queing.. ";
    this->getStreamKeyFromSongIDEx(songId);
}

void QPlayer::QPlayer::onBufferingProgress(int progress) {
    qDebug() << progress << "%...";

    // TODO: Fix this shit. For some reason on rPi you need to stop and replay
    // once the buffering is done. This must be a bug in the compile libraries for ARMv6.
    // This needs a fix!!!
    if (progress == 100) {
        this->player->play();
    }
}

