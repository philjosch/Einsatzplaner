#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "manager.h"
#include "filesettings.h"

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ExportDialog;
}

class ExportDialog: public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(Manager *m, FileSettings *settings, QWidget *parent);
    ~ExportDialog();

    void hardReload();

private slots:
    void on_pushDrucken_clicked();

    void on_comboVon_currentIndexChanged(int index);

    void on_comboBis_currentIndexChanged(int index);

    void show();

private:
    Ui::ExportDialog *ui;
    QWidget *p;
    Manager *manager;

    QMap<AActivity*, QListWidgetItem*> actToList;
    FileSettings *settings;

    bool testShow(AActivity *a);
};

#endif // EXPORTDIALOG_H
