#ifndef QREQUEST_H
#define QREQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


class QRequest : public QObject
{
    Q_OBJECT
private:
    const char *USER_AGENT = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_3) AppleWebKit/536.11 (KHTML, like Gecko) Chrome/20.0.1132.47 Safari/536.11";

    QMutex mutex;
    QNetworkAccessManager *manager;
    int currentPostActionId;

private slots:
    void dataReceived(QNetworkReply *reply);

public:
    explicit QRequest(QObject *parent = 0);
    static QString getMd5FromString(QString md5);
    static QString getSha1FromString(QString sha1string);
    void postData(QString url, QString data, int postActionId);

    // Static functions
    static QString jsonEncode(const QMap<QString, QVariant> &map);

    ~QRequest();

signals:
    void dataPosted(int postActionId, QString resultText);

};


#endif // QREQUEST_H
