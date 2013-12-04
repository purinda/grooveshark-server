#include "qrequest.h"
#include <QCryptographicHash>
#include <QByteArray>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QEventLoop>
#include <QVariant>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>


QRequest::QRequest(QObject *parent) :
    QObject(parent) {

    manager = new QNetworkAccessManager();
    currentPostActionId = -1;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(dataReceived(QNetworkReply*)));
}

QRequest::~QRequest() {
    delete manager;
}

QString QRequest::getMd5FromString(QString md5string) {
    return QString(QCryptographicHash::hash(md5string.toUtf8(), QCryptographicHash::Md5).toHex());
}

QString QRequest::getSha1FromString(QString sha1string) {
    return QString(QCryptographicHash::hash(sha1string.toUtf8(), QCryptographicHash::Sha1).toHex());
}

void QRequest::postData(QString url, QString data, int postActionId) {
    mutex.lock();
    currentPostActionId = postActionId;
    QUrl urlObj;
    urlObj.setUrl(url);

    QNetworkRequest request;
    request.setUrl(urlObj);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", QRequest::USER_AGENT);

    manager->post(request, data.toUtf8());
}

void QRequest::dataReceived(QNetworkReply *reply) {

    reply->deleteLater();
    QByteArray response = reply->readAll();
    qDebug() << "RESPONSE: " << response;
    reply->close();

    int clonedId = currentPostActionId;
    currentPostActionId = -1;

    mutex.unlock();
    emit dataPosted(clonedId, QString(response));
}


QString QRequest::jsonEncode(const QMap<QString, QVariant> &map) {

    QJsonObject jsonObj;

    foreach(QString key, map.keys()) {
        QJsonValue jsonValue = QJsonValue::fromVariant(map[key]);
        jsonObj.insert(key, jsonValue);
    }

    QJsonDocument jsonDoc(jsonObj);
    return QString(jsonDoc.toJson().simplified().replace(" ", ""));
}
