#ifndef QCONFIG_H
#define QCONFIG_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QtCore>

class QConfig : public QObject
{
    Q_OBJECT
public:
    explicit QConfig(QObject *parent = 0);

    static QConfig* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
                m_Instance = new QConfig(0);

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

    // Functions
    void initConfig(QString appDir);
    QVariant getSetting(QString setting, QVariant defaultVal);
    void writeSettings();
    void setSetting(QString setting, QVariant value);

private:
    QConfig() {}

    QConfig(const QConfig &); // hide copy constructor
    QConfig& operator=(const QConfig &);

    static QConfig* m_Instance;

    // Vars
    QSettings *qsettings;
};

#endif // QCONFIG_H
