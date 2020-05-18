#include "doomhub.h"
#include "ui_doomhub.h"

// TODO: Clean these up, and clean up namespace aliases
// TODO: Put util func in doomhub.
// TODO: Make util func platform independent with boost or qtlang
#include <experimental/filesystem>
#include "Util.h"
#include "QDirIterator"
#include <iostream>

namespace fs = std::experimental::filesystem;

DoomHub::DoomHub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoomHub)
{
    ui->setupUi(this);

    // TODO: Load resource file for engine path, wad path and default selections instead of using defaults

    // TODO: Make these something the user can edit
    const QString enginePath{"C:\\ManualPrograms\\Doom\\Engines"};
    const QString iWadPath{"C:\\ManualPrograms\\Doom\\Wads"};
    const QString archivePath{"C:\\ManualPrograms\\Doom\\Wads"};
    const QString customWadPath{"C:\\ManualPrograms\\Doom\\Wads"};

    // Archives and custom wads are optional
    archivePathLookup["(None)"] = "";
    customWadPathLookup["(None)"] = "";

    // TODO: Though the below works, this is slow now that you have changed things
    PopulateListWidget(*(ui->listWidgetEngines), enginePathLookup, enginePath, { "exe" });
    PopulateListWidget(*(ui->listWidgetIWads), iWadPathLookup, iWadPath, { "wad" });
    PopulateListWidget(*(ui->listWidgetArchives), archivePathLookup, archivePath,
        { "pk3", "pk7", "pkz", "pke", "ipk3", "ipk7" });
    PopulateListWidget(*(ui->listWidgetCustomWads), customWadPathLookup, customWadPath, { "wad" });

    BuildCommand();
}

DoomHub::~DoomHub()
{
    // TODO: Unique ptr?
    delete ui;
}

// TODO: Terrible lol, use a QTableView instead of a QListWidget to separate these
void DoomHub::PopulateListWidget(QListWidget& listWidget, std::map<QString, QString>& lookup, const QString& path, const QSet<QString>& extensions) {
    //listWidget.clear();
    //lookup.clear();

    for (QDirIterator dirIt(path, QDirIterator::Subdirectories); dirIt.hasNext();) {
        dirIt.next();
        if (extensions.contains(QFileInfo(dirIt.filePath()).suffix())) {
            QString fileName = dirIt.fileName();
            if (!lookup.emplace(fileName, dirIt.filePath()).second) {
                int i = 0;
                QString trialFileName;
                do {
                    ++i;
                    trialFileName = fileName + " (copy " + QVariant(i).toString() + ") ";
                } while (!lookup.emplace(trialFileName, dirIt.filePath()).second);
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
    QString command;

    // Engine
    const QList<QListWidgetItem*>& selectedEngines = ui->listWidgetEngines->selectedItems();
    if (!selectedEngines.empty()) {
        command += enginePathLookup[(*selectedEngines.begin())->text()];
    }

    // Archive
    const QList<QListWidgetItem*>& selectedArchives = ui->listWidgetArchives->selectedItems();
    if (!selectedArchives.empty()) {
        command += " " + archivePathLookup[(*selectedArchives.begin())->text()];
    }

    // WAD
    const QList<QListWidgetItem*>& selectedCustomWads = ui->listWidgetCustomWads->selectedItems();
    if (!selectedCustomWads.empty()) {
        command += " " + customWadPathLookup[(*selectedCustomWads.begin())->text()];
    }

    // IWAD
    const QList<QListWidgetItem*>& selectedIWads = ui->listWidgetIWads->selectedItems();
    if (!selectedIWads.empty()) {
        command += " -iwad " + iWadPathLookup[(*selectedIWads.begin())->text()];
    }

    ui->lineEditCommand->setText(command);
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
