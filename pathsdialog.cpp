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
    // TODO: unique_ptr?
    delete ui;
}

void PathsDialog::accept() {
    SavePaths();
    QDialog::accept();
}

void PathsDialog::LoadPaths()
{
    ui->lineEditEngine->setText(QString::fromStdString(paths.engine.string()));
    ui->lineEditIwad->setText(QString::fromStdString(paths.iwad.string()));
    ui->lineEditArchive->setText(QString::fromStdString(paths.archive.string()));
    ui->lineEditWad->setText(QString::fromStdString(paths.wad.string()));
}

void PathsDialog::SavePaths()
{
    paths.engine = ui->lineEditEngine->text().toStdString();
    paths.iwad = ui->lineEditIwad->text().toStdString();
    paths.archive = ui->lineEditArchive->text().toStdString();
    paths.wad = ui->lineEditWad->text().toStdString();
}

void PathsDialog::on_pushButtonEngine_clicked()
{
    paths.engine = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Engines", ui->lineEditEngine->text()).toStdString();
    if (!paths.engine.empty()) {
        ui->lineEditEngine->setText(QString::fromStdString(paths.engine.string()));
    }
}

void PathsDialog::on_pushButtonIwad_clicked()
{
    paths.iwad = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom IWADs", ui->lineEditIwad->text()).toStdString();
    if (!paths.iwad.empty()) {
        ui->lineEditIwad->setText(QString::fromStdString(paths.iwad.string()));
    }
}

void PathsDialog::on_pushButtonArchive_clicked()
{
    paths.archive = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Archives", ui->lineEditArchive->text()).toStdString();
    if (!paths.archive.empty()) {
        ui->lineEditArchive->setText(QString::fromStdString(paths.archive.string()));
    }
}

void PathsDialog::on_pushButtonWad_clicked()
{
    paths.wad = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom WADs", ui->lineEditWad->text()).toStdString();
    if (!paths.wad.empty()) {
        ui->lineEditWad->setText(QString::fromStdString(paths.wad.string()));
    }
}
