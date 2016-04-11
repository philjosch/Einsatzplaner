#ifndef RESERVIERUNGENUEBERSICHT_H
#define RESERVIERUNGENUEBERSICHT_H

#include <QMainWindow>
#include "managerreservierungen.h"

namespace Ui {
class ReservierungenUebersicht;
}

class ReservierungenUebersicht : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReservierungenUebersicht(ManagerReservierungen *manager, QWidget *parent = 0);
    ~ReservierungenUebersicht();

    void setManager(ManagerReservierungen *value);

public slots:
    void reservierungChanged();

private slots:
    void on_pushDrucken_clicked();
    void on_pushPDF_clicked();
    void on_radioListName_clicked();
    void on_radioListPlatz_clicked();

private:
    Ui::ReservierungenUebersicht *ui;
    ManagerReservierungen *manager;

    void cleanTable();
    void cleanGrafik();

    void setTable();
    void setGrafik();
};

#endif // RESERVIERUNGENUEBERSICHT_H
