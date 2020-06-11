#include "paths.h"
#include <QDir>

void Paths::LoadSettings(const QSettings& settings) {
    engines = settings.value("Paths/engines", QDir::rootPath()).toString().toStdString();
    iWads = settings.value("Paths/iwads", QDir::rootPath()).toString().toStdString();
    archives = settings.value("Paths/archives", QDir::rootPath()).toString().toStdString();
    customWads = settings.value("Paths/wads", QDir::rootPath()).toString().toStdString();
}

void Paths::SaveSettings(QSettings& settings) {
    settings.setValue("Paths/engines", QString::fromStdString(engines.string()));
    settings.setValue("Paths/iwads", QString::fromStdString(iWads.string()));
    settings.setValue("Paths/archives", QString::fromStdString(archives.string()));
    settings.setValue("Paths/wads", QString::fromStdString(customWads.string()));
}
