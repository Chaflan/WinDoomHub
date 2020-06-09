#ifndef PATHSDIALOG_H
#define PATHSDIALOG_H

#include <QDialog>

namespace Ui {
class PathsDialog;
}

class PathsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathsDialog(QWidget *parent = nullptr);
    ~PathsDialog();

private:
    Ui::PathsDialog *ui;
};

#endif // PATHSDIALOG_H
