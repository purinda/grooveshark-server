#include "qconfig.h"

QConfig* QConfig::m_Instance = 0;

QConfig::QConfig(QObject *parent) :
    QObject(parent)
{

}

void QConfig::initConfig(QString appDir) {
    this->qsettings = new QSettings(appDir + QDir::separator() + "settings.ini",
                                    QSettings::IniFormat);
}

void QConfig::setSetting(QString setting, QVariant value) {
    this->qsettings->setValue(setting, value);
}

void QConfig::writeSettings() {
    this->qsettings->sync();
}

QVariant QConfig::getSetting(QString setting, QVariant defaultVal) {
    return this->qsettings->value(setting, defaultVal);
}
