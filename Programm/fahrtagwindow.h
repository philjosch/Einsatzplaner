#ifndef FAHRTAGWINDOW_H
#define FAHRTAGWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QPushButton>

#include "fahrtag.h"

namespace Ui {
class FahrtagWindow;
}

class FahrtagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FahrtagWindow(QWidget *parent, Fahrtag *f);
    ~FahrtagWindow();

private slots:
    // Allgemeine Daten
    void on_dateDate_dateChanged(const QDate &date);
    void on_comboArt_currentIndexChanged(int index);
    void on_textAnlass_textChanged();
    void on_checkWichtig_stateChanged(int arg1);
    void on_comboWagenreihung_currentTextChanged(const QString &arg1);
    void on_comboTimeTfH_currentTextChanged(const QString &arg1);
    void on_comboTimeTfM_currentTextChanged(const QString &arg1);
    void on_comboTimeZH_currentTextChanged(const QString &arg1);
    void on_comboTimeZM_currentTextChanged(const QString &arg1);
    void on_comboTimeEndeH_currentTextChanged(const QString &arg1);
    void on_comboTimeEndeM_currentTextChanged(const QString &arg1);

    // Personaltab
    void on_listTf_itemChanged(QListWidgetItem *item);
    void on_buttonTfAdd_clicked();
    void on_buttonTfDelete_clicked();
    void on_checkTf_clicked(bool checked);

    void on_listZub_itemChanged(QListWidgetItem *item);
    void on_buttonZfAdd_clicked();
    void on_buttonZfDelete_clicked();
    void on_checkZf_clicked(bool checked);

    void on_listZf_itemChanged(QListWidgetItem *item);
    void on_buttonZubAdd_clicked();
    void on_buttonZubDelete_clicked();
    void on_checkZub_clicked(bool checked);

    void on_listService_itemChanged(QListWidgetItem *item);
    void on_buttonServiceAdd_clicked();
    void on_buttonServiceDelete_clicked();
    void on_checkService_clicked(bool checked);

    void on_textBemerkungen_textChanged();

    // Reservierungen
    void on_listRes_itemDoubleClicked(QListWidgetItem *item);
    void on_buttonShow_clicked();
    void on_buttonVerteile_clicked();
    void on_lineName_textChanged(const QString &arg1);
    void on_lineMail_textChanged(const QString &arg1);
    void on_lineTelefon_textChanged(const QString &arg1);
    void on_spinAnzahl_valueChanged(int arg1);
    void on_comboKlasse_currentIndexChanged(int index);
    void on_comboStart1Zug_currentTextChanged(const QString &arg1);
    void on_comboStart1Hp_currentTextChanged(const QString &arg1);
    void on_comboEnde1Zug_currentTextChanged(const QString &arg1);
    void on_comboEnde1Hp_currentTextChanged(const QString &arg1);
    void on_lineSitze_textChanged(const QString &arg1);
    void on_lineSitze_returnPressed();
    void on_checkFahrrad_clicked(bool checked);
    void on_plainSonstiges_textChanged();
    void on_listRes_itemClicked(QListWidgetItem *item);

    void update(); // Aktualisiert die Anzeige, wieviele Sitzplätze belegt sind

    // Einsatzzeiten
//    void on_timeEnde_timeChanged(const QTime &time);
    void on_checkBoxBenoetigt_clicked(bool checked);
    void on_tablePersonen_cellChanged(int row, int column);

    void on_buttonInsert_clicked();
    void on_buttonRemove_clicked();

    // Menue
    void on_actionDelete_triggered();
    void on_actionPrint_triggered();
    void on_actionPdf_triggered();
    void on_actionResPdf_triggered();
    void on_actionResPrint_triggered();

    // Hilfe
    void on_buttonAdd_clicked();
    void on_buttonDelete_clicked();

    void on_checkBoxAll_clicked(bool checked);

    void comboInTableChanged();
    void timeEditInTableChanged();

private:
    // Allgemeines
    Ui::FahrtagWindow *ui;
    Fahrtag *fahrtag;
    bool nehme;

    void loadData();

    // Personal Verwaltung
    QMap<QListWidgetItem *, QString> *listeMitNamen; // Liste mit den Namen, die in den Listen verwaltet werden
    QMap<QListWidgetItem*, QTableWidgetItem*> *listToTable; // Mapt von den Listen auf den Tabelleneintrag der Person
    QSet<QString> *namen;

    void addItemTolist(QListWidget *l, QPushButton *b);
    void deleteItemFromList(QListWidget *l, QPushButton *b);
    void itemChanged(QListWidgetItem *item, Category kat, bool isExtern=false);

    // Reservierungen
    QMap<Reservierung*, QListWidgetItem*> *resToItem;
    QMap<QListWidgetItem*, Reservierung*> *itemToRes;

    Reservierung *aktuelleRes;
    bool nehmeRes;

    void loadReservierung(Reservierung *r);
    void saveResFahrt();
};

#endif // FAHRTAGWINDOW_H
