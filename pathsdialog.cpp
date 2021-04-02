#include "pathsdialog.h"
#include "ui_pathsdialog.h"
#include <QFileDialog>

PathsDialog::PathsDialog(DirPaths& p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathsDialog),
    paths(p)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButtonEngine, &QPushButton::clicked, this, &PathsDialog::onPushButtonEngineClicked);
    QObject::connect(ui->pushButtonIWad, &QPushButton::clicked, this, &PathsDialog::onPushButtonIWadClicked);
    QObject::connect(ui->pushButtonArchive, &QPushButton::clicked, this, &PathsDialog::onPushButtonArchiveClicked);
    QObject::connect(ui->pushButtonWad, &QPushButton::clicked, this, &PathsDialog::onPushButtonCustomWadClicked);

    LoadPaths();
}

PathsDialog::~PathsDialog() {
    delete ui;
}

void PathsDialog::accept() {
    SavePaths();
    QDialog::accept();
}

void PathsDialog::LoadPaths() {
    ui->lineEditEngine->setText(paths.engines);
    ui->lineEditIWad->setText(paths.iWads);
    ui->lineEditArchive->setText(paths.archives);
    ui->lineEditCustomWad->setText(paths.customWads);
}

void PathsDialog::SavePaths() {
    paths.engines = ui->lineEditEngine->text();
    paths.iWads = ui->lineEditIWad->text();
    paths.archives = ui->lineEditArchive->text();
    paths.customWads = ui->lineEditCustomWad->text();
}

void PathsDialog::onPushButtonEngineClicked() {
    paths.engines = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Engines", ui->lineEditEngine->text());
    if (!paths.engines.isEmpty()) {
        ui->lineEditEngine->setText(paths.engines);
    }
}

void PathsDialog::onPushButtonIWadClicked() {
    paths.iWads = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom IWADs", ui->lineEditIWad->text());
    if (!paths.iWads.isEmpty()) {
        ui->lineEditIWad->setText(paths.iWads);
    }
}

void PathsDialog::onPushButtonArchiveClicked() {
    paths.archives = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom Archives", ui->lineEditArchive->text());
    if (!paths.archives.isEmpty()) {
        ui->lineEditArchive->setText(paths.archives);
    }
}

void PathsDialog::onPushButtonCustomWadClicked() {
    paths.customWads = QFileDialog::getExistingDirectory(this, "Select Directory Containing Doom WADs", ui->lineEditCustomWad->text());
    if (!paths.customWads.isEmpty()) {
        ui->lineEditCustomWad->setText(paths.customWads);
    }
}
