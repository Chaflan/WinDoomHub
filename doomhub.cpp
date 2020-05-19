#include "doomhub.h"
#include "ui_doomhub.h"

// TODO: Clean these up, and clean up namespace aliases
// TODO: Put util func in doomhub.
// TODO: Make util func platform independent with boost or qtlang
#include <filesystem>
#include "Util.h"
#include "QDirIterator"
#include <iostream>

namespace fs = std::filesystem;

DoomHub::DoomHub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoomHub)
{
    ui->setupUi(this);

    // TODO: Load resource file for engine path, wad path and default selections instead of using defaults

    // TODO: Make these something the user can edit
    const fs::path enginePath{"C:\\ManualPrograms\\Doom\\Engines"};
    const fs::path iWadPath{"C:\\ManualPrograms\\Doom\\Wads"};
    const fs::path archivePath{"C:\\ManualPrograms\\Doom\\Wads"};
    const fs::path customWadPath{"C:\\ManualPrograms\\Doom\\Wads"};

    // Archives and custom wads are optional
    archivePathLookup["(None)"] = "";
    customWadPathLookup["(None)"] = "";

    // TODO: Though the below works, this is slow now that you have changed things
    PopulateListWidget(*(ui->listWidgetEngines), enginePathLookup, enginePath, { ".exe" });
    PopulateListWidget(*(ui->listWidgetIWads), iWadPathLookup, iWadPath, { ".wad" });
    PopulateListWidget(*(ui->listWidgetArchives), archivePathLookup, archivePath,
        { ".pk3", ".pk7", ".pkz", ".pke", ".ipk3", ".ipk7" });
    PopulateListWidget(*(ui->listWidgetCustomWads), customWadPathLookup, customWadPath, { ".wad" });

    BuildCommand();
}

DoomHub::~DoomHub()
{
    // TODO: Unique ptr?
    delete ui;
}

// TODO: Terrible lol, use a QTableView instead of a QListWidget to separate these
void DoomHub::PopulateListWidget(QListWidget& listWidget, std::map<QString, fs::path>& lookup, const fs::path& path, const std::set<std::string>& extensions) {

    for (const auto& e : fs::recursive_directory_iterator(path)) {
        // TODO: Check performance by saving e.path() as a reference.
        if (extensions.find(e.path().extension().string()) != extensions.end()) {
            QString fileName = QString::fromStdString(e.path().filename().string());
            if (!lookup.emplace(fileName, e.path()).second) {
                int i = 0;
                QString trialFileName;
                do {
                    ++i;
                    trialFileName = fileName + " (copy " + QVariant(i).toString() + ") ";
                } while (!lookup.emplace(trialFileName, e.path()).second);
                fileName = trialFileName;
            }
        }
    }

    for (const auto& [fileName, filePath] : lookup) {
        listWidget.addItem(fileName);
    }
}

void DoomHub::PlayDoom() {
    Util::CreateProcessWrap("", ui->lineEditCommand->text().toStdString());
}

void DoomHub::BuildCommand() {
    std::string command;

    // Engine
    const QList<QListWidgetItem*>& selectedEngines = ui->listWidgetEngines->selectedItems();
    if (!selectedEngines.empty()) {
        command += enginePathLookup[(*selectedEngines.begin())->text()].string();
    }

    // Archive
    const QList<QListWidgetItem*>& selectedArchives = ui->listWidgetArchives->selectedItems();
    if (!selectedArchives.empty()) {
        command += " " + archivePathLookup[(*selectedArchives.begin())->text()].string();
    }

    // WAD
    const QList<QListWidgetItem*>& selectedCustomWads = ui->listWidgetCustomWads->selectedItems();
    if (!selectedCustomWads.empty()) {
        command += " " + customWadPathLookup[(*selectedCustomWads.begin())->text()].string();
    }

    // IWAD
    const QList<QListWidgetItem*>& selectedIWads = ui->listWidgetIWads->selectedItems();
    if (!selectedIWads.empty()) {
        command += " -iwad " + iWadPathLookup[(*selectedIWads.begin())->text()].string();
    }

    ui->lineEditCommand->setText(QString::fromStdString(command));
}

void DoomHub::on_pushButtonRun_clicked() {
    PlayDoom();
}

void DoomHub::on_listWidgetEngines_itemSelectionChanged() {
    BuildCommand();
}
void DoomHub::on_listWidgetIWads_itemSelectionChanged() {
    BuildCommand();
}
void DoomHub::on_listWidgetArchives_itemSelectionChanged() {
    BuildCommand();
}
void DoomHub::on_listWidgetCustomWads_itemSelectionChanged() {
    BuildCommand();
}
