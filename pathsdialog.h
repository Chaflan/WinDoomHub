#ifndef PATHSDIALOG_H
#define PATHSDIALOG_H

#include <QDialog>
#include "paths.h"

namespace Ui {
class PathsDialog;
}

class PathsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathsDialog(Paths& p, QWidget *parent = nullptr);
    ~PathsDialog();

public Q_SLOTS:
    virtual void accept() override;

private slots:
    void on_pushButtonEngine_clicked();
    void on_pushButtonIwad_clicked();
    void on_pushButtonArchive_clicked();
    void on_pushButtonWad_clicked();

private:
    void LoadPaths();
    void SavePaths();

private:
    Ui::PathsDialog *ui;

    Paths& paths;
};

#endif // PATHSDIALOG_H
