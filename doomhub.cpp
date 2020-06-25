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

    QSettings settings = GetSettings();
    LoadPathSettings(settings);
    PopulateListWidgets();
    LoadSelectionSettings(settings);
    BuildCommand();
}

DoomHub::~DoomHub()
{
    delete ui;
}

QSettings DoomHub::GetSettings() const {
    return QSettings("settings.ini", QSettings::IniFormat);
}

void DoomHub::LoadPathSettings() {
    QSettings settings = GetSettings();
    LoadPathSettings(settings);
}

void DoomHub::LoadPathSettings(const QSettings& settings) {
    paths.LoadSettings(settings);
}

void DoomHub::LoadSelectionSettings() {
    QSettings settings = GetSettings();
    LoadSelectionSettings(settings);
}

void DoomHub::LoadSelectionSettings(const QSettings& settings) {
    // Get the value associated with the settingKey from settings.ini.  Select this value from the lw.
    auto SelectWithSetting = [&settings = std::as_const(settings)] (const QString& settingKey, QListWidget& lw) {
        QString desiredSelection = settings.value(settingKey).toString();
        const auto& listWidgetList = lw.findItems(desiredSelection, Qt::MatchFlag::MatchExactly);
        if (!listWidgetList.empty()) {
            lw.setCurrentItem(listWidgetList.first());
        }
    };

    SelectWithSetting("Selections/engines", *(ui->listWidgetEngines));
    SelectWithSetting("Selections/iwads", *(ui->listWidgetIWads));
    SelectWithSetting("Selections/archives", *(ui->listWidgetArchives));
    SelectWithSetting("Selections/cwads", *(ui->listWidgetCustomWads));
}

void DoomHub::SavePathSettings() const {
    QSettings settings = GetSettings();
    paths.SaveSettings(settings);
}

void DoomHub::SaveSelectionSettings() const {
    QSettings settings = GetSettings();

    // Set the settings.ini value associated with the settingKey to be the current selection from the list widget
    auto SetSelectedAsSetting = [&settings](const QString& settingKey, QListWidget& lw){
        settings.setValue(settingKey, lw.selectedItems().empty() ? "" : (*lw.selectedItems().begin())->text());
    };

    SetSelectedAsSetting("Selections/engines", *(ui->listWidgetEngines));
    SetSelectedAsSetting("Selections/iwads", *(ui->listWidgetIWads));
    SetSelectedAsSetting("Selections/archives", *(ui->listWidgetArchives));
    SetSelectedAsSetting("Selections/cwads", *(ui->listWidgetCustomWads));
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
    auto PopulateListWidget = [](QListWidget& lw, const std::map<QString, QString>& lookup){
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

// TODO: Try to break your program

// Looks like you can remove the use of fs::path altogether from stored data and use it only in the below method.  QString here would mean
// Less foolishness.

// Populate the lookup mapping with all files in path (or its subdirectories) that have one of the extensions in the passed set
void DoomHub::PopulateLookup(std::map<QString, QString>& lookup, const QString& path, const std::set<std::string>& extensions) {
    lookup.clear();
    fs::path stlPath{ path.toStdString() };

    int i = 0;
    const int maxIter = 1000;
    std::set<QString> collisionNames;

    // Iterate through directories and subdirectories and map filename -> path.
    // Upon collision, map "filename (parent path)" -> path, and store
    // the collision name so that we can go back and change its mapping to "filename (parent path)" -> path too.
    // We don't do it now or we wouldn't know about future collisions with that name.
    for (const auto& e : fs::recursive_directory_iterator(stlPath, fs::directory_options::skip_permission_denied)) {
        if (extensions.find(e.path().extension().string()) != extensions.end()) {
            QString fileName = QString::fromStdString(e.path().filename().string());
            QString mappedPath = QString::fromStdString(e.path().lexically_normal().string());
            if (!lookup.emplace(fileName, mappedPath).second) {
                QString newFileName{ fileName + " (" + QString::fromStdString(e.path().parent_path().lexically_normal().string()) + ")" };
                lookup.emplace(std::move(newFileName), std::move(mappedPath));
                collisionNames.emplace(std::move(fileName));
            }
        }

        if (++i >= maxIter) {
            break;
        }
    }

    // Fix collision names
    for (const auto& name : collisionNames) {
        auto nodeHandle = lookup.extract(name);
        fs::path mappedPath{ nodeHandle.mapped().toStdString() };
        nodeHandle.key() = name + " (" + QString::fromStdString(mappedPath.parent_path().lexically_normal().string()) + ")";
        lookup.insert(std::move(nodeHandle));
    }
}

void DoomHub::BuildCommand() {
    QString command;

    auto AddToCommandString = [&command](
            const QString& prefix,
            const QListWidget& lw,
            const std::map<QString, QString>& lookup){
        if (!lw.selectedItems().empty()) {
            command += prefix;
            command += lookup.at(lw.selectedItems().first()->text());
        }
    };

    AddToCommandString("", (*ui->listWidgetEngines), enginePathLookup);
    AddToCommandString(" ", (*ui->listWidgetArchives), archivePathLookup);
    AddToCommandString(" ", (*ui->listWidgetCustomWads), customWadPathLookup);
    AddToCommandString(" -wad ", (*ui->listWidgetIWads), iWadPathLookup);

    ui->plainTextEditCommand->setPlainText(command);
}

void DoomHub::PlayDoom() const {
    Util::ExecuteCommandLine(ui->plainTextEditCommand->toPlainText().toStdString());
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

// Click the settings > path button:
// Open the path dialog.  Do nothing on cancel. On ok do the following:
//      - Unselect all in all list widgets
//      - Repopulate them with the (presumably) new paths
//      - Save these new paths to settings.ini
void DoomHub::on_actionPaths_triggered() {
    PathsDialog p(paths);
    p.setModal(true);
    p.exec();

    if (p.result() == QDialog::DialogCode::Accepted) {

        auto UnselectAllFromWidget = [](QListWidget& lw){
            for (const auto& item : lw.selectedItems()) {
                lw.setItemSelected(item, false);
            }
        };

        UnselectAllFromWidget(*(ui->listWidgetEngines));
        UnselectAllFromWidget(*(ui->listWidgetIWads));
        UnselectAllFromWidget(*(ui->listWidgetArchives));
        UnselectAllFromWidget(*(ui->listWidgetCustomWads));

        PopulateListWidgets();
        SavePathSettings();
    }
}
