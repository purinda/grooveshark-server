#include "qsearch.h"

QSearch::QSearch(QObject *parent) : QObject(parent)
{
    this->gsSession = new QGrooveSession();
    this->gsRequest = new QRequest();

    connect(this->gsSession, SIGNAL(tokenDataLoaded()),
            this, SLOT(onTokenDataLoaded()));

    connect(this->gsRequest, SIGNAL(dataPosted(int, QString)),
            this, SLOT(onResponse(int, QString)));

}


void QSearch::getStreamKeyFromSongIDEx(int songId) {
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

    qDebug() << "Request URL: " << gsSession->HOST + gsSession->getEndpoint() + "?" + GS_FUNCTION << endl;
    qDebug() << "Request: " << QRequest::jsonEncode(request) << endl;

    gsRequest->postData(gsSession->HOST + gsSession->getEndpoint() + "?" + GS_FUNCTION, QRequest::jsonEncode(request), 1);
}

void QSearch::onTokenDataLoaded() {
    // Token data ready in GrooveShark Session
    // Test 1 - Load Song
    getStreamKeyFromSongIDEx(33364907);
}

void QSearch::onResponse(int postActionId, QString response) {
    qDebug() << "Response: " << response;
}
