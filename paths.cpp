#include "paths.h"
#include <QDir>

void Paths::LoadSettings(const QSettings& settings) {
    engines = settings.value("Paths/engines", QDir::rootPath()).toString();
    iWads = settings.value("Paths/iwads", QDir::rootPath()).toString();
    archives = settings.value("Paths/archives", QDir::rootPath()).toString();
    customWads = settings.value("Paths/cwads", QDir::rootPath()).toString();
}

void Paths::SaveSettings(QSettings& settings) const {
    settings.setValue("Paths/engines", engines);
    settings.setValue("Paths/iwads", iWads);
    settings.setValue("Paths/archives", archives);
    settings.setValue("Paths/cwads", customWads);
}
