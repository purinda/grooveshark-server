#ifndef QGROOVESESSION_H
#define QGROOVESESSION_H

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "qrequest.h"
#include "qconfig.h"

class QGrooveSession : public QObject
{
private:
    Q_OBJECT

    QNetworkAccessManager* nam;
    QString gsSessionId;
    QString gsToken;
    QJsonObject gsCountry;
    QString gsEndpoint;
    QString gsHttpHost;
    QRequest *gsRequest;
    bool sessionActive = false;

private slots:
    void onLoadTokenDataSlot(QNetworkReply* reply);
    void loadCommunicationToken(QString tokenResponse);

    // Function to split responses
    void onResponse(int postActionId, QString response);

public:
    const QString SALT       = ":tastyTacos:";
    const QString USER_AGENT = "htmlshark";
    const QString HOST       = "https://grooveshark.com/";
    const QString TOKEN_URL  = "https://grooveshark.com/preload.php?getCommunicationToken=1";

    explicit QGrooveSession(QObject *parent = 0);

    // Getters
    QString getSessionId();
    QString getToken();
    QString getEndpoint();
    QString getHost();
    QString getCountryJsonString();
    QJsonObject getCountryJsonObject();

    bool isActive();
    void loadTokenData();
    void getCommunicationToken();
    QMap<QString, QVariant> getHeaderMap(QString method);

signals:
    void tokenDataLoaded();
};

#endif // QGROOVESESSION_H
