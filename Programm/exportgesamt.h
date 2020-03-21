#ifndef EXPORTGESAMT_H
#define EXPORTGESAMT_H

#include "manager.h"
#include "managerfilesettings.h"

#include <QDialog>

namespace Ui {
class ExportGesamt;
}

class ExportGesamt : public QDialog
{
    Q_OBJECT

public:
    explicit ExportGesamt(Manager *m, ManagerFileSettings *settings, QWidget *parent = nullptr);
    ~ExportGesamt();

    void reload();
    void hardReload();

private slots:
    void on_pushDrucken_clicked();

    void on_comboVon_currentIndexChanged(int index);

    void on_dateVon_dateChanged();

    void on_comboBis_currentIndexChanged(int index);

    void on_dateBis_dateChanged();

    void on_comboFahrtag_currentIndexChanged();

    void on_checkActivity_clicked();

    void on_pushButton_clicked();

private:
    Ui::ExportGesamt *ui;
    QWidget *p;
    Manager *manager;

    void show();

    QList<AActivity*> liste;
    QMap<AActivity*, QListWidgetItem*> actToList;
    QMap<QListWidgetItem*, AActivity*> listToAct;
    ManagerFileSettings *settings;

    bool testShow(AActivity *a);
};

#endif // EXPORTGESAMT_H
