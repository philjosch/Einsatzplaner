#ifndef BEITRAEGEEDITORDIALOG_H
#define BEITRAEGEEDITORDIALOG_H

#include <QDialog>
#include <managerpersonal.h>

namespace Ui {
class BeitraegeEditorDialog;
}

class BeitraegeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BeitraegeEditorDialog(QWidget *parent, ManagerPersonal *manager);
    ~BeitraegeEditorDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::BeitraegeEditorDialog *ui;

    ManagerPersonal *managerPersonal;
};

#endif // BEITRAEGEEDITORDIALOG_H
