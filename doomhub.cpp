#include "doomhub.h"
#include "ui_doomhub.h"

// TODO: Clean these up, and clean up namespace aliases
// TODO: Put util func in doomhub.
// TODO: Make util func platform independent with boost or qtlang
#include <filesystem>
#include "Util.h"
#include "QDirIterator"
#include <iostream>
#include "pathsdialog.h"
#include "qmessagebox.h"
#include <QSettings>
#include <algorithm>
#include <QDebug>

namespace fs = std::filesystem;

DoomHub::DoomHub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoomHub)
{
    ui->setupUi(this);
    LoadSettings();
    PopulateListWidgets();
    BuildCommand();
}

DoomHub::~DoomHub()
{
    // TODO: Unique ptr?
    delete ui;
}

void DoomHub::LoadSettings() {
    QSettings settings("settings.ini", QSettings::IniFormat);
    paths.LoadSettings(settings);
}

void DoomHub::SaveSettings() {
    QSettings settings("settings.ini", QSettings::IniFormat);
    paths.SaveSettings(settings);
}

void DoomHub::PopulateListWidgets() {
    enginePathLookup.clear();
    iWadPathLookup.clear();
    archivePathLookup.clear();
    customWadPathLookup.clear();

    // Archives and custom wads are optional
    archivePathLookup["(None)"] = "";
    customWadPathLookup["(None)"] = "";

    // TODO: Though the below works, this is slow now that you have changed things
    PopulateListWidget(*(ui->listWidgetEngines), enginePathLookup, paths.engine, { ".exe" });
    PopulateListWidget(*(ui->listWidgetIWads), iWadPathLookup, paths.iwad, { ".wad" });
    PopulateListWidget(*(ui->listWidgetArchives), archivePathLookup, paths.archive,
        { ".pk3", ".pk7", ".pkz", ".pke", ".ipk3", ".ipk7" });
    PopulateListWidget(*(ui->listWidgetCustomWads), customWadPathLookup, paths.wad, { ".wad" });
}

// TODO: Split this into two methods, populate the map, populate the list widget
void DoomHub::PopulateListWidget(QListWidget& listWidget, std::map<QString, fs::path>& lookup, const fs::path& path, const std::set<std::string>& extensions) {

    std::set<QString> collisionNames;

    int i = 0;
    const int maxIter = 1000;

    // Iterate through directories and subdirectories and map filename -> path.  Upon collision, map "filename (parent path)" -> path, and store
    // the collision name so that we can go back and change its mapping to "filename (parent path)" -> path too.  We don't do it now or we wouldn't
    // know about future collisions with that name.
    for (const auto& e : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
        if (extensions.find(e.path().extension().string()) != extensions.end()) {
            QString fileName = QString::fromStdString(e.path().filename().string());
            if (!lookup.emplace(fileName, e.path()).second) {
                QString newFileName = fileName + " (" + QString::fromStdString(e.path().parent_path().string()) + ")";
                lookup.emplace(std::move(newFileName), e.path());
                collisionNames.emplace(std::move(fileName));
            }
        }

        if (++i >= maxIter) {
            break;
        }
    }

    for (const auto& name : collisionNames) {
        auto nodeHandle = lookup.extract(name);
        nodeHandle.key() = name + " (" + QString::fromStdString(path.parent_path().string()) + ")";
        lookup.insert(std::move(nodeHandle));
    }

    listWidget.clear();
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

void DoomHub::on_actionPaths_triggered() {
    PathsDialog p(paths);
    p.setModal(true);
    p.exec();

    if (p.result() == QDialog::DialogCode::Accepted) {
        PopulateListWidgets();
        SaveSettings();
    }
}
