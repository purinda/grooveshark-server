#include "qconfig.h"

QConfig* QConfig::m_Instance = 0;

QConfig::QConfig(QObject *parent) :
    QObject(parent)
{

}

void QConfig::initConfig(QCoreApplication *app) {
    this->qsettings = new QSettings(app->applicationDirPath() + QDir::separator() + "settings.xml",
                                    QSettings::NativeFormat);
}
