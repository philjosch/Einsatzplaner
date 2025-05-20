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
    void changedFrom(int index);

    void changedTill(int index);

    void show();
    void showPrintPreview();

    void exportUpload();
    void exportPDF();
    void exportPrint();

private:
    Ui::ExportDialog *ui;
    QWidget *p;
    Manager *manager;

    QMap<AActivity*, QListWidgetItem*> actToList;
    FileSettings *settings;

    bool testShow(AActivity *a);
    QList<AActivity*> getAActivityForExport(bool ignoreSelectionStatus = false);
};

#endif // EXPORTDIALOG_H
