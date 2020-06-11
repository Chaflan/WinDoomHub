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

QSettings DoomHub::GetSettings() {
    return QSettings("settings.ini", QSettings::IniFormat);
}

void DoomHub::LoadPathSettings() {
    QSettings settings = GetSettings();
    paths.LoadSettings(settings);
}

void DoomHub::LoadSelectionSettings() {
    QSettings settings = GetSettings();

    // Get the value associated with the key from settings.ini.  Select this value from the lw.
    auto selectWithSetting = [&settings = std::as_const(settings)] (const QString& settingKey, QListWidget& lw) {
        QString desiredSelection = settings.value(settingKey).toString();
        const auto& listWidgetList = lw.findItems(desiredSelection, Qt::MatchFlag::MatchExactly);
        if (!listWidgetList.empty()) {
            lw.setCurrentItem(listWidgetList.first());
        }
    };

    selectWithSetting("Selections/engines", *(ui->listWidgetEngines));
    selectWithSetting("Selections/iwads", *(ui->listWidgetIWads));
    selectWithSetting("Selections/archives", *(ui->listWidgetArchives));
    selectWithSetting("Selections/wads", *(ui->listWidgetCustomWads));
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

void DoomHub::PopulateListWidgets() {

    PopulateLookup(enginePathLookup, paths.engines, { ".exe" });
    PopulateLookup(iWadPathLookup, paths.iWads, { ".wad" });
    PopulateLookup(archivePathLookup, paths.archives, { ".pk3", ".pk7", ".pkz", ".pke", ".ipk3", ".ipk7" });
    PopulateLookup(customWadPathLookup, paths.customWads, { ".wad" });

    // Archives and custom wads are optional
    archivePathLookup["(None)"] = "";
    customWadPathLookup["(None)"] = "";

    // Populate lw with the keys of the lookup
    auto PopulateListWidget = [](QListWidget& lw, const std::map<QString, fs::path>& lookup){
        lw.clear();
        for (const auto& [fileName, filePath] : lookup) {
            lw.addItem(fileName);
        }
    };

    PopulateListWidget(*(ui->listWidgetEngines), enginePathLookup);
    PopulateListWidget(*(ui->listWidgetIWads), iWadPathLookup);
    PopulateListWidget(*(ui->listWidgetArchives), archivePathLookup);
    PopulateListWidget(*(ui->listWidgetCustomWads), customWadPathLookup);
}

void DoomHub::PopulateLookup(std::map<QString, fs::path>& lookup, const fs::path& path, const std::set<std::string>& extensions) {
    lookup.clear();

    int i = 0;
    const int maxIter = 1000;
    std::vector<QString> collisionNames;

    // Iterate through directories and subdirectories and map filename -> path.
    // Upon collision, map "filename (parent path)" -> path, and store
    // the collision name so that we can go back and change its mapping to "filename (parent path)" -> path too.
    // We don't do it now or we wouldn't know about future collisions with that name.
    for (const auto& e : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
        if (extensions.find(e.path().extension().string()) != extensions.end()) {
            QString fileName = QString::fromStdString(e.path().filename().string());
            if (!lookup.emplace(fileName, e.path()).second) {
                QString newFileName = fileName + " (" + QString::fromStdString(e.path().parent_path().string()) + ")";
                lookup.emplace(std::move(newFileName), e.path());
                collisionNames.emplace_back(std::move(fileName));
            }
        }

        if (++i >= maxIter) {
            break;
        }
    }

    // Fix collision names
    for (const auto& name : collisionNames) {
        auto nodeHandle = lookup.extract(name);
        nodeHandle.key() = name + " (" + QString::fromStdString(path.parent_path().string()) + ")";
        lookup.insert(std::move(nodeHandle));
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
