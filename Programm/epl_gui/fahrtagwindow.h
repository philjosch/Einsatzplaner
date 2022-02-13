#ifndef FAHRTAGWINDOW_H
#define FAHRTAGWINDOW_H

#include "fahrtag.h"
#include "coremainwindow.h"
#include "guihelper.h"

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QListWidgetItem>

namespace Ui {
class FahrtagWindow;
}

class FahrtagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FahrtagWindow(CoreMainWindow *parent, Fahrtag *f);
    ~FahrtagWindow();

private slots:
    // Allgemeine Daten
    void changedDate(const QDate &date);
    void changedType(int index);
    void changedPurpose();
    void changedImportant(bool checked);
    void changedCancelled(bool checked);
    void changedCarOrder(const QString &arg1);
    void changedTimeStartTf(const QTime &time);
    void changedTimeStart(const QTime &time);
    void changedTimeEnd(const QTime &time);
    void changedTimeStatus(bool checked);

    // Personaltab
    void changedTf(QListWidgetItem *item);
    void addTf();
    void deleteTf();
    void changedRequireTf(int button);

    void changedZf(QListWidgetItem *item);
    void addZf();
    void deleteZf();
    void changedRequireZf(bool checked);

    void changedZub(QListWidgetItem *item);
    void addZub();
    void deleteZub();
    void changedRequireZub(bool checked);

    void changedService(QListWidgetItem *item);
    void addService();
    void deleteService();
    void changedRequireService(bool checked);

    void changedDescription();

    // Reservierungen
    void resAdd();
    void resDelete();

    void resChangedFilter(int index);

    void resChangedCheckAll(bool checked);
    void resDistribute();

    void resNameChanged(const QString &arg1);
    void resMailChanged(const QString &arg1);
    void resTelefonChanged(const QString &arg1);

    void resChangedNumber(int arg1);
    void resChangedClass(int index);
    void resChangedSeats(const QString &arg1);
    void resChangedSeatsCheck();
    void resChangedBike(bool checked);

    void handlerFahrtChanged();

    void resChangedComment();
    void resItemClicked(QListWidgetItem *item);

    void resUpdateStatistics(); // Aktualisiert die Anzeige, wieviele Sitzplätze belegt sind

    // Einsatzzeiten
    void changedRequired(bool checked);
    void changedTableCell(int row, int column);

    void removePersonTabelle();
    EinsatzTableWidgetItem *addRowToTable();
    EinsatzTableWidgetItem *addToTable(Einsatz *e=nullptr, bool block=false);

    // Menue
    void deleteTriggered();
    void exportPrint();
    void exportPdf();
    void exportReservationPdf();
    void exportReservationPrint();

    // Hilfe

signals:
    void loeschen(AActivity*);

private:
    // Allgemeines
    Ui::FahrtagWindow *ui;
    Fahrtag *fahrtag;
    bool nehme;

    void addItemTolist(QListWidget *l, QPushButton *b);
    void deleteItemFromList(QListWidget *l, QPushButton *b);
    void itemInListChanged(QListWidgetItem *item, Category kat);

    // Reservierungen
    void plausibilityCheck();
    QMap<QListWidgetItem*, Reservierung*> itemToRes;

    Reservierung *aktuelleRes;
    bool nehmeRes;

    void showReservierung(Reservierung *r);
    void saveResFahrt();
    void toggleFelderReservierung(bool enabled);

    void updateWindowTitle();

    int getIndexVonZug(int zug);
    int getZugVonIndex(int index);
    int getHpVonIndex(int index);

    QString getBelegungVonKlasseUndZug(Fahrtag *f, int zug, int klasse);

};

#endif // FAHRTAGWINDOW_H
