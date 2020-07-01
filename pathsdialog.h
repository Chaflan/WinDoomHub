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
    void on_pushButtonEngine_clicked();
    void on_pushButtonIWad_clicked();
    void on_pushButtonArchive_clicked();
    void on_pushButtonCustomWad_clicked();

private:
    void LoadPaths();
    void SavePaths();

private:
    Ui::PathsDialog *ui;
    DirPaths& paths;
};

#endif // PATHSDIALOG_H
