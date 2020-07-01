#ifndef DOOMHUB_H
#define DOOMHUB_H

#include "dirpaths.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <map>
#include <set>

QT_BEGIN_NAMESPACE
namespace Ui { class DoomHub; }
QT_END_NAMESPACE

class DoomHub : public QMainWindow {

    Q_OBJECT

public:
    DoomHub(QWidget* parent = nullptr);
    ~DoomHub();

private:
    QSettings GetSettings() const;
    void LoadPathSettings();
    void LoadPathSettings(const QSettings& settings);
    void LoadSelectionSettings();
    void LoadSelectionSettings(const QSettings& settings);
    void SavePathSettings() const;
    void SaveSelectionSettings() const;

    void PopulateListWidgets();
    static void PopulateLookup(
            std::map<QString, QString>& lookup,
            const QString& path,
            const std::set<QString>& extensions);
    void BuildCommand();
    void PlayDoom() const;
    static void ExecuteCommandLine(const std::string& command);

private slots:
    void on_pushButtonRun_clicked();
    void on_listWidgetEngines_itemSelectionChanged();
    void on_listWidgetIWads_itemSelectionChanged();
    void on_listWidgetArchives_itemSelectionChanged();
    void on_listWidgetCustomWads_itemSelectionChanged();
    void on_actionPaths_triggered();

private:
    Ui::DoomHub* ui;

    DirPaths dirPaths;

    // These map filename -> absolute path
    std::map<QString, QString> enginePathLookup;
    std::map<QString, QString> iWadPathLookup;
    std::map<QString, QString> archivePathLookup;
    std::map<QString, QString> customWadPathLookup;
};
#endif // DOOMHUB_H
