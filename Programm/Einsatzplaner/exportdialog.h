#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "manager.h"
#include "activityfiltermodel.h"
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

private slots:
    void changedFrom();
    void changedTill();
    void changedType();

    void showPrintPreview();

    void exportUpload();
    void exportPDF();
    void exportPrint();

private:
    Ui::ExportDialog *ui;
    Manager *manager;
    ActivityFilterModel *filterModel;

    FileSettings *settings;

    QList<AActivity*> getAActivityForExport(bool ignoreSelectionStatus = false);
};

#endif // EXPORTDIALOG_H
