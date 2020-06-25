#include "pathsdialog.h"
#include "ui_pathsdialog.h"

#include "qfiledialog.h"

PathsDialog::PathsDialog(Paths& p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathsDialog),
    paths(p)
{
    ui->setupUi(this);
    LoadPaths();
}

PathsDialog::~PathsDialog()
{
    delete ui;
}

void PathsDialog::accept() {
    SavePaths();
    QDialog::accept();
}

void PathsDialog::LoadPaths()
{
    ui->lineEditEngine->setText(paths.engines);
    ui->lineEditIWad->setText(paths.iWads);
    ui->lineEditArchive->setText(paths.archives);
    ui->lineEditCustomWad->setText(paths.customWads);
}

void PathsDialog::SavePaths()
{
    paths.engines = ui->lineEditEngine->text();
    paths.iWads = ui->lineEditIWad->text();
    paths.archives = ui->lineEditArchive->text();
    paths.customWads = ui->lineEditCustomWad->text();
}

void PathsDialog::on_pushButtonEngine_clicked()
{
    paths.engines = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Engines", ui->lineEditEngine->text());
    if (!paths.engines.isEmpty()) {
        ui->lineEditEngine->setText(paths.engines);
    }
}

void PathsDialog::on_pushButtonIWad_clicked()
{
    paths.iWads = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom IWADs", ui->lineEditIWad->text());
    if (!paths.iWads.isEmpty()) {
        ui->lineEditIWad->setText(paths.iWads);
    }
}

void PathsDialog::on_pushButtonArchive_clicked()
{
    paths.archives = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Archives", ui->lineEditArchive->text());
    if (!paths.archives.isEmpty()) {
        ui->lineEditArchive->setText(paths.archives);
    }
}

void PathsDialog::on_pushButtonCustomWad_clicked()
{
    paths.customWads = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom WADs", ui->lineEditCustomWad->text());
    if (!paths.customWads.isEmpty()) {
        ui->lineEditCustomWad->setText(paths.customWads);
    }
}
