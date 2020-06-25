#ifndef PATHS_H
#define PATHS_H

#include <filesystem>
#include <QSettings>

namespace fs = std::filesystem;

struct Paths
{
    void LoadSettings(const QSettings& settings);
    void SaveSettings(QSettings& settings) const;

    // TODO: Debate the merits of using a QString or QPath

    QString engines;
    QString iWads;
    QString archives;
    QString customWads;
};

#endif // PATHS_H
