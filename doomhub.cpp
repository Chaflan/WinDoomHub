#include "doomhub.h"
#include "ui_doomhub.h"
#include "pathsdialog.h"
#include <QDirIterator>
#include <windows.h>
#include <unordered_set>

//------------------------------------------------------------------------------------------------------------------------------------
// Template specialization for storing QStrings in STL hash tables.
//------------------------------------------------------------------------------------------------------------------------------------
namespace std {
    template<>
    struct hash<QString> {
        size_t operator()(const QString& str) const {
            return qHash(str);
        }
    };
}

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

DoomHub::~DoomHub() {
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
    dirPaths.LoadSettings(settings);
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
    dirPaths.SaveSettings(settings);
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
    PopulateLookup(enginePathLookup, dirPaths.engines, { "exe" });
    PopulateLookup(iWadPathLookup, dirPaths.iWads, { "wad" });
    PopulateLookup(archivePathLookup, dirPaths.archives, { "pk3", "pk7", "pkz", "pke", "ipk3", "ipk7" });
    PopulateLookup(customWadPathLookup, dirPaths.customWads, { "wad" });

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

//------------------------------------------------------------------------------------------------------------------------------------
// Populate the lookup mapping with all files in rootDirPath (or its subdirectories) that have one of the extensions in the passed set
//------------------------------------------------------------------------------------------------------------------------------------
void DoomHub::PopulateLookup(std::map<QString, QString>& lookup, const QString& rootDirPath, const std::set<QString>& extensions) {
    lookup.clear();

    // Set of file names that collided; had two keys/names of the same value.
    std::unordered_set<QString> collisions;

    // Every non colliding name will use this to map "file name" -> "canonical path (path without filename)".
    std::unordered_map<QString, QString> canonicalPathLookup;

    // Iterate through rootDirPath and subdirs.  Map "file name" -> "full path".  If a key collision happens map "file name (canonical path)" -> "full path".
    // Store the name in collisions so we can go back and re-key the node we collided with.  We don't do it now in case of future collisions with that name.
    // All non-collisions store canonical path in canonicalPathLookup for this purpose.
    QDirIterator it(rootDirPath, QDir::Files, QDirIterator::Subdirectories);
    for(int i = 0; it.hasNext() && i < 1000; ++i) {
        QString mapFullPath = QDir::toNativeSeparators(it.next());
        QFileInfo fileInfo = it.fileInfo();

        if (extensions.find(fileInfo.suffix()) != extensions.end()) {
            QString keyFileName = fileInfo.fileName();
            if (lookup.emplace(keyFileName, mapFullPath).second) {
                canonicalPathLookup.emplace(std::move(keyFileName), fileInfo.canonicalPath());
            } else {
                QString keyNewFileName = keyFileName  + " (" + QDir::toNativeSeparators(fileInfo.canonicalPath()) + ")";
                lookup.emplace(std::move(keyNewFileName), std::move(mapFullPath));
                collisions.emplace(std::move(keyFileName));
            }
        }
    }

    // Fix collision names
    for (const QString& fileName : collisions) {
        auto nodeHandle = lookup.extract(fileName);
        nodeHandle.key() = fileName + " (" + QDir::toNativeSeparators(canonicalPathLookup.at(fileName)) + ")";
        auto ret = lookup.insert(std::move(nodeHandle));
    }
}

void DoomHub::BuildCommand() {
    QString command;

    auto AddToCommandString = [&command](
            const QLatin1Literal& prefix,
            const QListWidget& lw,
            const std::map<QString, QString>& lookup){
        if (!lw.selectedItems().empty()) {
            command += prefix;
            command += lookup.at(lw.selectedItems().first()->text());
        }
    };

    AddToCommandString(QLatin1Literal{ "" }, (*ui->listWidgetEngines), enginePathLookup);
    AddToCommandString(QLatin1Literal{ " " }, (*ui->listWidgetArchives), archivePathLookup);
    AddToCommandString(QLatin1Literal{ " " }, (*ui->listWidgetCustomWads), customWadPathLookup);
    AddToCommandString(QLatin1Literal{ " -wad " }, (*ui->listWidgetIWads), iWadPathLookup);

    ui->plainTextEditCommand->setPlainText(command);
}

void DoomHub::PlayDoom() const {
    ExecuteCommandLine(ui->plainTextEditCommand->toPlainText().toStdString());
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

//------------------------------------------------------------------------------------------------------------------------------------
// Click the settings > path button:
// Open the path dialog.  Do nothing on cancel. On ok do the following:
//      - Unselect all in all list widgets
//      - Repopulate them with the (presumably) new paths
//      - Save these new paths to settings.ini
//------------------------------------------------------------------------------------------------------------------------------------
void DoomHub::on_actionPaths_triggered() {
    PathsDialog p(dirPaths);
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

//------------------------------------------------------------------------------------------------------------------------------------
// Execute "commmand" as if it were a string written into the cmd console.  This only works in windows.
//------------------------------------------------------------------------------------------------------------------------------------
void DoomHub::ExecuteCommandLine(const std::string& command)
{
    // We need to convert to non-const for CreateProcessA
    char argsCStr[2048];
    strcpy_s(argsCStr, 2048, command.c_str());

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcessA
    (
        NULL,               // the path to executable
        argsCStr,           // arguments, or full command
        NULL,               // Process handle not inheritable
        NULL,               // Thread handle not inheritable
        FALSE,              // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, // Opens file in a separate console
        NULL,               // Use parent's environment block
        NULL,               // Use parent's starting directory
        &si,                // Pointer to STARTUPINFO structure
        &pi                 // Pointer to PROCESS_INFORMATION structure
    );

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
