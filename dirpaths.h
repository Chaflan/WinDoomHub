#ifndef PATHS_H
#define PATHS_H

#include <QSettings>

//---------------------------------------------------------------------------------------------------
// DirPaths is a group of Strings containing absolute paths to directories containing doom files.
// These are intended to be loaded and saved to QSettings objects and have such supporting functions.
//---------------------------------------------------------------------------------------------------
struct DirPaths {

    void LoadSettings(const QSettings& settings);
    void SaveSettings(QSettings& settings) const;

    QString engines;
    QString iWads;
    QString archives;
    QString customWads;
};

#endif // PATHS_H
