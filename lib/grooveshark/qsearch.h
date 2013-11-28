#ifndef QSEARCH_H
#define QSEARCH_H

#include "QtCore"
#include <QObject>
#include "qrequest.h"
#include "qgroovesession.h"

class QSearch : public QObject
{
private:
    Q_OBJECT

    QGrooveSession *gsSession;
    QRequest *gsRequest;

private slots:
    void onTokenDataLoaded();
    void onResponse(int postActionId, QString response);

public:
    explicit QSearch(QObject *parent = 0);

    void getStreamKeyFromSongIDEx(int songId);
};

#endif // QSEARCH_H
