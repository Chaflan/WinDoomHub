#ifndef DOOMHUB_H
#define DOOMHUB_H

#include <QMainWindow>
#include <map>
#include "QListWidgetItem"

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
    static void PopulateListWidget(
            QListWidget& listWidget,
            std::map<QString, QString>& lookup,
            const QString& path,
            const QSet<QString>& extensions);

    void PlayDoom();

private slots:
    // TODO: Try [[maybe_unused]] here?
    void on_listWidgetWads_itemDoubleClicked(QListWidgetItem* /*Unused*/);
    void on_listWidgetArchives_itemDoubleClicked(QListWidgetItem* /*Unused*/);
    void on_listWidgetEngines_itemDoubleClicked(QListWidgetItem* /*Unused*/);

private:
    Ui::DoomHub *ui;

    // TODO: check this
    QString engineDirPath;
    QString engineSelection;
    QString wadDirPath;
    QString wadSelection;

    std::map<QString, QString> enginePathLookup;
    std::map<QString, QString> archivePathLookup;
    std::map<QString, QString> wadPathLookup;

    std::map<std::string, std::string> pathLookup;
};
#endif // DOOMHUB_H
