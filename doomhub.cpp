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
    LoadPathSettings();
    PopulateListWidgets();
    LoadSelectionSettings();
    BuildCommand();
}

DoomHub::~DoomHub()
{
    delete ui;
}

void DoomHub::LoadPathSettings() {
    QSettings settings = GetSettings();
    paths.LoadSettings(settings);
}

void DoomHub::LoadSelectionSettings() {
    QSettings settings = GetSettings();

    auto selectWithString = [&settings = std::as_const(settings)] (const QString& s, QListWidget& lw) {
        QString desiredSelection = settings.value(s).toString();
        const auto& listWidgetList = lw.findItems(desiredSelection, Qt::MatchFlag::MatchExactly);
        if (!listWidgetList.empty()) {
            lw.setCurrentItem(listWidgetList.first());
        }
    };

    selectWithString("Selections/engines", *(ui->listWidgetEngines));
    selectWithString("Selections/iwads", *(ui->listWidgetIWads));
    selectWithString("Selections/archives", *(ui->listWidgetArchives));
    selectWithString("Selections/wads", *(ui->listWidgetCustomWads));
}

void DoomHub::SavePathSettings() {
    QSettings settings = GetSettings();
    paths.SaveSettings(settings);
}

void DoomHub::SaveSelectionSettings() {
    const QList<QListWidgetItem*>& selectedEngines = ui->listWidgetEngines->selectedItems();
    const QList<QListWidgetItem*>& selectedIWads = ui->listWidgetIWads->selectedItems();
    const QList<QListWidgetItem*>& selectedArchives = ui->listWidgetArchives->selectedItems();
    const QList<QListWidgetItem*>& selectedCustomWads = ui->listWidgetCustomWads->selectedItems();

    QSettings settings = GetSettings();
    settings.setValue("Selections/engines", selectedEngines.empty() ? "" : (*selectedEngines.begin())->text());
    settings.setValue("Selections/iwads", selectedIWads.empty() ? "" : (*selectedIWads.begin())->text());
    settings.setValue("Selections/archives", selectedArchives.empty() ? "" : (*selectedArchives.begin())->text());
    settings.setValue("Selections/wads", selectedCustomWads.empty() ? "" : (*selectedCustomWads.begin())->text());
}

QSettings DoomHub::GetSettings() {
    return QSettings("settings.ini", QSettings::IniFormat);
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
    PopulateListWidget(*(ui->listWidgetEngines), enginePathLookup, paths.engines, { ".exe" });
    PopulateListWidget(*(ui->listWidgetIWads), iWadPathLookup, paths.iWads, { ".wad" });
    PopulateListWidget(*(ui->listWidgetArchives), archivePathLookup, paths.archives,
        { ".pk3", ".pk7", ".pkz", ".pke", ".ipk3", ".ipk7" });
    PopulateListWidget(*(ui->listWidgetCustomWads), customWadPathLookup, paths.customWads, { ".wad" });
}

// TODO: Split this into two methods, populate the map, populate the list widget
void DoomHub::PopulateListWidget(QListWidget& listWidget, std::map<QString, fs::path>& lookup, const fs::path& path, const std::set<std::string>& extensions) {

    // TODO: Vector?  You don't really need the set functionality for this
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

    // TODO: string_view?
    auto AddToCommandString = [&command](
            const std::string& prefix,
            const QListWidget& lw,
            const std::map<QString, fs::path>& lookup){
        if (!lw.selectedItems().empty()) {
            command += prefix + lookup.at(lw.selectedItems().first()->text()).string();
        }
    };

    AddToCommandString("", (*ui->listWidgetEngines), enginePathLookup);
    AddToCommandString(" ", (*ui->listWidgetArchives), archivePathLookup);
    AddToCommandString(" ", (*ui->listWidgetCustomWads), customWadPathLookup);
    AddToCommandString(" -wad ", (*ui->listWidgetIWads), iWadPathLookup);

    ui->lineEditCommand->setText(QString::fromStdString(command));
}

void DoomHub::on_pushButtonRun_clicked() {
    SaveSelectionSettings();
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
        SavePathSettings();
    }
}
