#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include "managerpersonal.h"

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPrinter>
#include <QTableWidgetItem>
#include <coremainwindow.h>

namespace Ui {
class PersonalWindow;
}

class PersonalWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonalWindow(CoreMainWindow *parent, ManagerPersonal *m);
    ~PersonalWindow();

public slots:
    void refresh();

private slots:
    void refreshTabelle();
    void refreshEinzel();

    // Personalmanager
    void on_actionPersonAdd_triggered();
    void on_actionMindeststunden_triggered();

    // Export
    void on_actionZeitenEinzelEinzelPDF_triggered();
    void on_actionZeitenEinzelEinzelDrucken_triggered();

    void on_actionZeitenEinzelListePDF_triggered();
    void on_actionZeitenEinzelListeDrucken_triggered();

    void on_actionZeitenListePDF_triggered();
    void on_actionZeitenListeDrucken_triggered();


    void on_actionMitgliederEinzelEinzelPDF_triggered();
    void on_actionMitgliederEinzelEinzelDrucken_triggered();

    void on_actionMitgliederEinzelListePDF_triggered();
    void on_actionMitgliederEinzelListeDrucken_triggered();

    void on_actionMitgliederListePDF_triggered();
    void on_actionMitgliederListeDrucken_triggered();
    void on_actionMitgliederListeCSV_triggered();


    // Fenster - Gesamt
    void on_pushEmail_clicked();

    void on_tabelleGesamt_cellDoubleClicked(int row, int column);

    void on_checkShowGesamt_clicked(bool checked);
    void on_checkShowAnzahl_clicked(bool checked);
    void on_checkShowTf_clicked(bool checked);
    void on_checkShowTb_clicked(bool checked);
    void on_checkShowZf_clicked(bool checked);
    void on_checkShowZub_clicked(bool checked);
    void on_checkShowService_clicked(bool checked);
    void on_checkShowVorbereiten_clicked(bool checked);
    void on_checkShowWerkstatt_clicked(bool checked);
    void on_checkShowBuero_clicked(bool checked);
    void on_checkShowAusbildung_clicked(bool checked);
    void on_checkShowInfrastruktur_clicked(bool checked);
    void on_checkShowSonstiges_clicked(bool checked);
    void on_checkShowKilometer_clicked(bool checked);


    // Fenster - Einzel

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_lineVorname_textChanged(const QString &arg1);
    void on_lineNachname_textChanged(const QString &arg1);

    void on_checkAktiv_clicked(bool checked);

    void on_spinKm_valueChanged(int arg1);

    void on_checkTf_clicked(bool checked);
    void on_checkZf_clicked(bool checked);
    void on_checkRangierer_clicked(bool checked);

    void on_dateDienst_dateChanged(const QDate &date);
    void on_checkDienst_clicked(bool checked);

    void on_plainBemerkung_textChanged();
    void on_plainAusbildung_textChanged();
    void on_plainBetrieb_textChanged();

    void on_pushDelete_clicked();


    void on_lineTf_textChanged(const QString &arg1);
    void on_lineTb_textChanged(const QString &arg1);
    void on_lineZf_textChanged(const QString &arg1);
    void on_lineZub_textChanged(const QString &arg1);
    void on_lineService_textChanged(const QString &arg1);
    void on_lineZugVorbereiten_textChanged(const QString &arg1);
    void on_lineWerkstatt_textChanged(const QString &arg1);
    void on_lineBuero_textChanged(const QString &arg1);
    void on_lineAusbildung_textChanged(const QString &arg1);
    void on_lineInfrastruktur_textChanged(const QString &arg1);
    void on_lineSonstiges_textChanged(const QString &arg1);

    void on_doubleAnzahl_valueChanged(double arg1);
    void on_doubleKilometer_valueChanged(double arg1);


    void showPerson(Person *p);

    // Mitglieder
    void on_pushMailEinzel_clicked();

private:
    Ui::PersonalWindow *ui;
    ManagerPersonal *manager;

    CoreMainWindow *parentWindow;
    Person *aktuellePerson;
    QList<Person*> current;
    Status filter;
    QHash<Person*, QListWidgetItem*> personToItem;
    QSet<Category> anzeige;

    bool enabled; // Gibt an, ob das Formular aktiviert ist oder nicht, und ob Änderungen übernommen werden

    QList<Person*> getSortierteListe();

    void toggleFields(bool state);

    void setZeitenNachVeraenderung(Category cat, QString arg);

    void updateZeiten();
    void faerbeZelle(QTableWidgetItem *item, QString hintergrund, QString vordergrund = "black");
};
#endif // PERSONALWINDOW_H
