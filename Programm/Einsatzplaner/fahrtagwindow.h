#ifndef FAHRTAGWINDOW_H
#define FAHRTAGWINDOW_H

#include "fahrtag.h"
#include "coremainwindow.h"

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

    class EinsatzTableWidgetItem : public QTableWidgetItem {
    public:
        EinsatzTableWidgetItem(QString s) : QTableWidgetItem(s)
        {
            einsatz = AActivity::Einsatz();
        }
        EinsatzTableWidgetItem(AActivity::Einsatz e) : QTableWidgetItem()
        {
            einsatz = e;
        }

        AActivity::Einsatz getEinsatz() const
        {
            return einsatz;
        }
        void setEinsatz(AActivity::Einsatz value)
        {
            einsatz = value;
        }

    protected:
        AActivity::Einsatz einsatz;
    };
    class TableListWidgetItem : public QListWidgetItem {
    public:
        TableListWidgetItem(QString s) : QListWidgetItem(s)
        {
            tableItem = nullptr;
        }
        TableListWidgetItem() : QListWidgetItem()
        {
            tableItem = nullptr;
        }

        EinsatzTableWidgetItem *getTableItem() const
        {
            return tableItem;
        }
        void setTableItem(EinsatzTableWidgetItem *value)
        {
            tableItem = value;
        }

    protected:
        EinsatzTableWidgetItem *tableItem;
    };


private slots:
    // Allgemeine Daten
    void on_dateDate_dateChanged(const QDate &date);
    void on_comboArt_currentIndexChanged(int index);
    void on_textAnlass_textChanged();
    void on_checkWichtig_clicked(bool checked);
    void on_checkAbgesagt_clicked(bool checked);
    void on_comboWagenreihung_currentTextChanged(const QString &arg1);
    void on_comboTimeTfH_currentTextChanged(const QString &arg1);
    void on_comboTimeTfM_currentTextChanged(const QString &arg1);
    void on_comboTimeZH_currentTextChanged(const QString &arg1);
    void on_comboTimeZM_currentTextChanged(const QString &arg1);
    void on_comboTimeEndeH_currentTextChanged(const QString &arg1);
    void on_comboTimeEndeM_currentTextChanged(const QString &arg1);
    void on_checkZeiten_clicked(bool checked);

    // Personaltab
    void on_listTf_itemChanged(QListWidgetItem *item);
    void on_buttonTfAdd_clicked();
    void on_buttonTfDelete_clicked();
    void on_buttonGroupTf_buttonClicked(int button);

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
    void on_buttonAddReservierung_clicked();
    void on_buttonDeleteReservierung_clicked();

    void on_comboAuswahlRes_currentIndexChanged(int index);

    void on_checkBoxAll_clicked(bool checked);
    void on_buttonVerteile_clicked();

    void on_lineName_textChanged(const QString &arg1);
    void on_lineMail_textChanged(const QString &arg1);
    void on_lineTelefon_textChanged(const QString &arg1);

    void on_spinAnzahl_valueChanged(int arg1);
    void on_comboKlasse_currentIndexChanged(int index);
    void on_lineSitze_textChanged(const QString &arg1);
    void on_lineSitze_returnPressed();
    void on_checkFahrrad_clicked(bool checked);

    void handlerFahrtChanged();

    void on_plainSonstiges_textChanged();
    void on_listRes_itemClicked(QListWidgetItem *item);

    void updateAuswertungReservierungen(); // Aktualisiert die Anzeige, wieviele Sitzplätze belegt sind

    // Einsatzzeiten
    void on_checkBoxBenoetigt_clicked(bool checked);
    void on_tablePersonen_cellChanged(int row, int column);

    void on_buttonRemovePerson_clicked();
    EinsatzTableWidgetItem *fuegeZeileInTabelleEin(QString name="", Category kat=Sonstiges, QString bemerkung="", bool block=false, QTime beginn=QTime(0,0), QTime ende=QTime(0,0));

    // Menue
    void on_actionDelete_triggered();
    void on_actionPrint_triggered();
    void on_actionPdf_triggered();
    void on_actionResPdf_triggered();
    void on_actionResPrint_triggered();

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
