#ifndef PATHSDIALOG_H
#define PATHSDIALOG_H

#include "dirpaths.h"
#include <QDialog>

namespace Ui {
class PathsDialog;
}

class PathsDialog : public QDialog {

    Q_OBJECT

public:
    explicit PathsDialog(DirPaths& p, QWidget* parent = nullptr);
    ~PathsDialog();

public Q_SLOTS:
    virtual void accept() override;

private slots:
    void onPushButtonEngineClicked();
    void onPushButtonIWadClicked();
    void onPushButtonArchiveClicked();
    void onPushButtonCustomWadClicked();

private:
    void LoadPaths();
    void SavePaths();

private:
    Ui::PathsDialog *ui;
    DirPaths& paths;
};

#endif // PATHSDIALOG_H
