#include "qgroovesession.h"

QGrooveSession::QGrooveSession(QObject *parent) : QObject(parent) {
    this->nam = new QNetworkAccessManager();
    this->gsRequest = new QRequest();

    connect(this->nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onLoadTokenDataSlot(QNetworkReply*)));

    connect(this->gsRequest, SIGNAL(dataPosted(int, QString)),
            this, SLOT(onResponse(int, QString)));

    QUrl url(QGrooveSession::TOKEN_URL);
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
}

void QGrooveSession::onLoadTokenDataSlot(QNetworkReply* reply) {

    // Read existing settings
    QString existingSessionId     = QConfig::instance()->getSetting("Grooveshark/session", "").toString();
    QString existingEndpoint      = QConfig::instance()->getSetting("Grooveshark/endpoint", "").toString();
    QJsonDocument existingCountry = QJsonDocument::fromJson(QConfig::instance()->getSetting("Grooveshark/country", "")
                                                            .toByteArray());
    QDateTime lastrun             = QConfig::instance()->getSetting("Grooveshark/lastsession", QDateTime::currentDateTime()).toDateTime();

    // Check existing session is more than 7 days old if so renew else keep using it...
    bool useExistingSession = false;
    if (lastrun.addDays(7) > QDateTime::currentDateTime()) {
        useExistingSession = true;
    }

    // Set settings read and emit getCommunicationToken
    if (useExistingSession && existingSessionId.length() > 1) {
        this->gsSessionId = existingSessionId;
        this->gsEndpoint  = existingEndpoint;
        this->gsCountry   = existingCountry.object();

        this->getCommunicationToken();
        return;
    }

    // no error ?
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        QString rawTokenContent(bytes);

        // Get token data JSON
        QRegExp tokenRegex( "window.tokenData = (.*);" );

        if (tokenRegex.indexIn(rawTokenContent) != -1) {
            QJsonDocument jsonDocument = QJsonDocument::fromJson(tokenRegex.cap(1).toUtf8());
            QJsonObject parentObject = jsonDocument.object();
            QJsonValue value = parentObject.value(QString("getGSConfig"));

            // retrieve tokendata from Grooveshark
            this->gsSessionId = value.toObject()["sessionID"].toString();
            this->gsEndpoint  = value.toObject()["endpoint"].toString();
            this->gsCountry   = value.toObject()["country"].toObject();

            // Save retrieved settings
            QConfig::instance()->setSetting("Grooveshark/session", this->gsSessionId);
            QConfig::instance()->setSetting("Grooveshark/endpoint", this->gsEndpoint);
            QConfig::instance()->setSetting("Grooveshark/country", this->getCountryJsonString());
            QConfig::instance()->setSetting("Grooveshark/lastsession", QDateTime::currentDateTime());
        }

        qDebug() << "Session ID: " << this->gsSessionId << endl;
        qDebug() << "End point: " << this->gsEndpoint << endl;
        qDebug() << "Country : " << QString(QJsonDocument(this->gsCountry).toJson()) << endl;
    }
    else
    {
        // Dump the error message
        qDebug() << reply->errorString() << endl;
    }


    delete reply;

    // Get communication token using other data received.
    this->getCommunicationToken();
}

void QGrooveSession::loadCommunicationToken(QString tokenResponse) {

    QJsonDocument jsonDocument = QJsonDocument::fromJson(tokenResponse.toUtf8());
    QJsonObject parentObject = jsonDocument.object();
    QJsonValue value = parentObject.value(QString("result"));

    // retrieve tokendata from Grooveshark
    this->gsToken = value.toString();

    qDebug() << "Communication Token: " << this->gsToken << endl;
}

QMap<QString, QVariant> QGrooveSession::getHeaderMap(QString method) {
    QMap<QString, QVariant> headerMap;

//    headerMap["uuid"]           = QUuid::createUuidV3().;
    headerMap["country"]        = this->getCountryJsonString();
    headerMap["session"]        = this->gsSessionId;
    headerMap["client"]         = QGrooveSession::USER_AGENT;
    headerMap["clientRevision"] = "20120830";
    headerMap["privacy"]        = "0";


    srand(QTime::currentTime().msec());
    QByteArray randomHex;
    for (int i = 0; i < 6; i++) {
        int random = rand();

        while (random > 15) {
            random = random - 16;
        }

        randomHex += QString::number(random, 16);
    }

    QCryptographicHash sha1(QCryptographicHash::Sha1);
    sha1.addData(method.toUtf8() +
                 QString(":").toUtf8() +
                 this->gsToken.toUtf8() +
                 QGrooveSession::SALT.toUtf8() +
                 randomHex);

    QString token = QString(randomHex) + QString(sha1.result().toHex());

    // Display calculated string for the token
    qDebug() << "GS Token: " << this->gsToken << endl;
    qDebug() << "Calculated Random Hex: " << randomHex << endl;
    qDebug() << "Calculated SHA1: " << sha1.result().toHex() << endl;
    qDebug() << "Calculated Token: " << token << endl;

    headerMap["token"] = token;

    return headerMap;
}


void QGrooveSession::getCommunicationToken() {
    const QString GS_FUNCTION = "getCommunicationToken";

    // Generate secretKey
    QCryptographicHash secretKey(QCryptographicHash::Md5);
    secretKey.addData(this->gsSessionId.toUtf8());

    QMap<QString, QVariant> parameters;
    parameters["secretKey"] = secretKey.result().toHex();

    // Get header map and generate JSON to complete the request
    QMap<QString, QVariant> request;
    request["parameters"] = parameters;
    request["header"]     = this->getHeaderMap(GS_FUNCTION);
    request["method"]     = GS_FUNCTION;

    gsRequest->postData(this->HOST + this->getEndpoint() + "?" + GS_FUNCTION, QRequest::jsonEncode(request), 2);
}


QString QGrooveSession::getSessionId() {
    return this->getSessionId();
}

QString QGrooveSession::getToken() {
    return this->gsToken;
}

QString QGrooveSession::getEndpoint() {
    return this->gsEndpoint;
}

QString QGrooveSession::getHost() {
    return this->gsHttpHost;
}

QString QGrooveSession::getCountryJsonString() {
    return QString(QJsonDocument(this->gsCountry).toJson()).simplified().replace(" ", "");
}

QJsonObject QGrooveSession::getCountryJsonObject() {
    return this->gsCountry;
}

bool QGrooveSession::isActive() {
    return this->sessionActive;
}

void QGrooveSession::onResponse(int postActionId, QString response) {
    switch (postActionId) {
    case 2:
        this->loadCommunicationToken(response);
        emit tokenDataLoaded();
        sessionActive = true;
        break;

    }
}
