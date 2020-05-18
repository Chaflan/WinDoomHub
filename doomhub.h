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

    // TODO: Why did you use static here before?  Revisit this later.
    static void PopulateListWidget(
            QListWidget& listWidget,
            std::map<QString, QString>& lookup,
            const QString& path,
            const QSet<QString>& extensions);

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

    std::map<QString, QString> enginePathLookup;
    std::map<QString, QString> iWadPathLookup;
    std::map<QString, QString> archivePathLookup;
    std::map<QString, QString> customWadPathLookup;
};
#endif // DOOMHUB_H
