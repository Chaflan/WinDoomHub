#include "paths.h"
#include <QDir>

void Paths::LoadSettings(const QSettings& settings) {
    engine = settings.value("Paths/engines", QDir::rootPath()).toString().toStdString();
    iwad = settings.value("Paths/iwads", QDir::rootPath()).toString().toStdString();
    archive = settings.value("Paths/archives", QDir::rootPath()).toString().toStdString();
    wad = settings.value("Paths/wads", QDir::rootPath()).toString().toStdString();
}

void Paths::SaveSettings(QSettings& settings) {
    settings.beginGroup("Paths");
    settings.setValue("engines", QString::fromStdString(engine.string()));
    settings.setValue("iwads", QString::fromStdString(iwad.string()));
    settings.setValue("archives", QString::fromStdString(archive.string()));
    settings.setValue("wads", QString::fromStdString(wad.string()));
    settings.endGroup();
}
