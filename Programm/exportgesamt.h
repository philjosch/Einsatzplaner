#ifndef EXPORTGESAMT_H
#define EXPORTGESAMT_H

#include "manager.h"
#include "managerfilesettings.h"

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ExportGesamt;
}

class ExportGesamt : public QDialog
{
    Q_OBJECT

public:
    explicit ExportGesamt(Manager *m, ManagerFileSettings *settings, QWidget *parent);
    ~ExportGesamt();

    void hardReload();

private slots:
    void on_pushDrucken_clicked();

    void on_comboVon_currentIndexChanged(int index);

    void on_comboBis_currentIndexChanged(int index);

    void show();

private:
    Ui::ExportGesamt *ui;
    QWidget *p;
    Manager *manager;

    QList<AActivity*> liste;
    QMap<AActivity*, QListWidgetItem*> actToList;
    ManagerFileSettings *settings;

    bool testShow(AActivity *a);
};

#endif // EXPORTGESAMT_H
