#include "pathsdialog.h"
#include "ui_pathsdialog.h"

PathsDialog::PathsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathsDialog)
{
    ui->setupUi(this);
}

PathsDialog::~PathsDialog()
{
    delete ui;
}
