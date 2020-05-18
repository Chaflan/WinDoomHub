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
    // TODO: Consider configurability for exe, wad and ipk
    // TODO: Though the below works, this is very slow now that you have changed things

    const QString defaultParentPath = "C:\\ManualPrograms\\Doom";
    ui->lineEditParentPath->setText(defaultParentPath);

    archivePathLookup["(None)"] = "";
    wadPathLookup["(None)"] = "";

    PopulateListWidget(*(ui->listWidgetEngines), enginePathLookup, defaultParentPath, { "exe" });
    PopulateListWidget(*(ui->listWidgetArchives), archivePathLookup, defaultParentPath,
        { "pk3", "pk7", "pkz", "pke", "ipk3", "ipk7" });
    PopulateListWidget(*(ui->listWidgetWads), wadPathLookup, defaultParentPath, { "wad" });
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
    const QList<QListWidgetItem*>& selectedEngines = ui->listWidgetEngines->selectedItems();
    if (!selectedEngines.empty()) {
        std::string engineStr = enginePathLookup[(*selectedEngines.begin())->text()].toStdString();

        std::string argStr;
        const QList<QListWidgetItem*>& selectedArchives = ui->listWidgetArchives->selectedItems();
        const QList<QListWidgetItem*>& selectedWads = ui->listWidgetWads->selectedItems();

        if (!selectedArchives.empty()) {
            argStr += archivePathLookup[(*selectedArchives.begin())->text()].toStdString();
        }
        if (!selectedWads.empty()) {
            argStr += argStr.empty() ? "" : " ";
            argStr += wadPathLookup[(*selectedWads.begin())->text()].toStdString();
        }

        // HACK, bigtime
        argStr += " -iwad C:/ManualPrograms/Doom/Engines/PRBoom+/prboom-plus-2.5.1.4/doom2.wad";

        std::cout << "engineStr=" << engineStr << std::endl << "argStr=" << argStr << std::endl;
        Util::CreateProcessWrap(engineStr, argStr);
    }
}

void DoomHub::on_listWidgetWads_itemDoubleClicked(QListWidgetItem* /*Unused*/) {
    PlayDoom();
}

void DoomHub::on_listWidgetArchives_itemDoubleClicked(QListWidgetItem* /*Unused*/) {
    PlayDoom();
}

void DoomHub::on_listWidgetEngines_itemDoubleClicked(QListWidgetItem* /*Unused*/) {
    PlayDoom();
}
