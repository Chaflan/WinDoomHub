#ifndef DOOMHUB_H
#define DOOMHUB_H

#include <QMainWindow>
#include "QListWidgetItem"
#include <map>
#include <set>
#include <filesystem>
#include "paths.h"

namespace fs = std::filesystem;

QT_BEGIN_NAMESPACE
namespace Ui { class DoomHub; }
QT_END_NAMESPACE

class DoomHub : public QMainWindow
{
    Q_OBJECT

public:
    DoomHub(QWidget *parent = nullptr);
    ~DoomHub();

private:
    // TODO: Consts

    QSettings GetSettings();
    void LoadPathSettings();
    void LoadSelectionSettings();
    void SavePathSettings();
    void SaveSelectionSettings();

    void PopulateListWidgets();
    static void PopulateLookup(
            std::map<QString, fs::path>& lookup,
            const fs::path& path,
            const std::set<std::string>& extensions);

    void BuildCommand();
    void PlayDoom();

private slots:
    void on_pushButtonRun_clicked();
    void on_listWidgetEngines_itemSelectionChanged();
    void on_listWidgetIWads_itemSelectionChanged();
    void on_listWidgetArchives_itemSelectionChanged();
    void on_listWidgetCustomWads_itemSelectionChanged();

    void on_actionPaths_triggered();

private:
    Ui::DoomHub *ui;

    Paths paths;
    std::map<QString, fs::path> enginePathLookup;
    std::map<QString, fs::path> iWadPathLookup;
    std::map<QString, fs::path> archivePathLookup;
    std::map<QString, fs::path> customWadPathLookup;
};
#endif // DOOMHUB_H
