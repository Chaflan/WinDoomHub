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
    ui->lineEditEngine->setText(QString::fromStdString(paths.engines.string()));
    ui->lineEditIWad->setText(QString::fromStdString(paths.iWads.string()));
    ui->lineEditArchive->setText(QString::fromStdString(paths.archives.string()));
    ui->lineEditCustomWad->setText(QString::fromStdString(paths.customWads.string()));
}

void PathsDialog::SavePaths()
{
    paths.engines = ui->lineEditEngine->text().toStdString();
    paths.iWads = ui->lineEditIWad->text().toStdString();
    paths.archives = ui->lineEditArchive->text().toStdString();
    paths.customWads = ui->lineEditCustomWad->text().toStdString();
}

void PathsDialog::on_pushButtonEngine_clicked()
{
    paths.engines = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Engines", ui->lineEditEngine->text()).toStdString();
    if (!paths.engines.empty()) {
        ui->lineEditEngine->setText(QString::fromStdString(paths.engines.string()));
    }
}

void PathsDialog::on_pushButtonIWad_clicked()
{
    paths.iWads = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom IWADs", ui->lineEditIWad->text()).toStdString();
    if (!paths.iWads.empty()) {
        ui->lineEditIWad->setText(QString::fromStdString(paths.iWads.string()));
    }
}

void PathsDialog::on_pushButtonArchive_clicked()
{
    paths.archives = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Archives", ui->lineEditArchive->text()).toStdString();
    if (!paths.archives.empty()) {
        ui->lineEditArchive->setText(QString::fromStdString(paths.archives.string()));
    }
}

void PathsDialog::on_pushButtonCustomWad_clicked()
{
    paths.customWads = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom WADs", ui->lineEditCustomWad->text()).toStdString();
    if (!paths.customWads.empty()) {
        ui->lineEditCustomWad->setText(QString::fromStdString(paths.customWads.string()));
    }
}
