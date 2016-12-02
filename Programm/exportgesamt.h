#ifndef EXPORTGESAMT_H
#define EXPORTGESAMT_H

#include <QDialog>

#include "manager.h"

namespace Ui {
class ExportGesamt;
}

class ExportGesamt : public QDialog
{
    Q_OBJECT

public:
    explicit ExportGesamt(Manager *m, QWidget *parent = 0);
    ~ExportGesamt();

    void reload();
    void hardReload();

private slots:
    void on_pushDrucken_clicked();

    void on_comboVon_currentIndexChanged(int index);

    void on_dateVon_dateChanged(const QDate &date);

    void on_comboBis_currentIndexChanged(int index);

    void on_dateBis_dateChanged(const QDate &date);

    void on_comboFahrtag_currentIndexChanged(int index);

    void on_checkActivity_clicked(bool checked);

    void on_pushButton_clicked();

private:
    Ui::ExportGesamt *ui;
    QWidget *p;
    Manager *manager;

    void show();

    QList<AActivity*> liste;
    QMap<AActivity*, QListWidgetItem*> actToList;
    QMap<QListWidgetItem*, AActivity*> listToAct;

    bool testShow(AActivity *a);
};

#endif // EXPORTGESAMT_H
