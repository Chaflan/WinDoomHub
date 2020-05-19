#ifndef DOOMHUB_H
#define DOOMHUB_H

#include <QMainWindow>
#include "QListWidgetItem"
#include <map>
#include <set>
#include <filesystem>

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
    void ReadResourceFile();
    void WriteResourceFile();

    // TODO: Why did you use static here before?  Revisit this later.
    static void PopulateListWidget(
            QListWidget& listWidget,
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

private:
    Ui::DoomHub *ui;

    std::map<QString, fs::path> enginePathLookup;
    std::map<QString, fs::path> iWadPathLookup;
    std::map<QString, fs::path> archivePathLookup;
    std::map<QString, fs::path> customWadPathLookup;
};
#endif // DOOMHUB_H
