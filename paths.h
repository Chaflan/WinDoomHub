#ifndef PATHS_H
#define PATHS_H

#include <filesystem>
#include <QSettings>

namespace fs = std::filesystem;

struct Paths
{
    void LoadSettings(const QSettings& settings);
    void SaveSettings(QSettings& settings);

    // TODO: Debate the merits of using a QString or QPath
    // TODO: pluralize these with a refactor, matches settings and sounds better

    fs::path engine;
    fs::path iwad;
    fs::path archive;
    fs::path wad;
};

#endif // PATHS_H
