#ifndef QSERVERCHILDTHREAD_H
#define QSERVERCHILDTHREAD_H

#include <QObject>

class QServerChildThread : public QObject
{
    Q_OBJECT
public:
    explicit QServerChildThread(QObject *parent = 0);

signals:

public slots:

};

#endif // QSERVERCHILDTHREAD_H
