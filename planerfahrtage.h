#ifndef PLANERFAHRTAGE_H
#define PLANERFAHRTAGE_H
#include <QListWidgetItem>
#include <QMainWindow>
#include <QListWidgetItem>
#include <managerzuege.h>
#include "reservierungenuebersicht.h"

namespace Ui {
class PlanerFahrtage;
}

class PlanerFahrtage : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlanerFahrtage(QWidget *parent = 0);
    ~PlanerFahrtage();
    static QString getArt(int i);
    static QColor getFarbe(int i);

private:
    // private Variablen
    Ui::PlanerFahrtage *ui;
    bool saved;
    QString path;
    bool uebernehmen;
    Fahrtag *aktuellerZug;
    ManagerZuege *zugManager;
    ReservierungenUebersicht *windowReservierungen;


    // Erstellen von Fahrtagen und Aufruf zum Bearbeiten
    void ZugLaden(QListWidgetItem *zug);

    void ZugSichern();

    // Verändern der Rahmendaten
    void saveRahmendaten();

    // Verändern des Fahrplans
    void saveFahrplan();

    // Verändern des Personals
    void savePersonal();

    // Verändern der Reservierungen
    void saveReservierungen();

    // Ein-/Ausgabe
    void setSaved(bool save);

    void oeffnen();
    void speichern();
    void speichernUnter();

    QJsonObject toJson();
    void fromJson(QJsonObject o);

    // Hilfsmethoden
    void createEmptyForm();

    void setState(bool aktiv);
    void setStateRes(bool aktiv);

    QList<QString> *getQListFromListWidget(QListWidget *listWidget);
    void makeItemEditable(QListWidgetItem *item);
    void qListToListWidget(QList<QString> *liste, QListWidget *listWidget);

    void deleteSelectedRowFromListWidget(QListWidget* curr);
    void addEmptyRowToListWidget(QListWidget* curr);

    void closeEinsatzPlaner();
    void closeEvent(QCloseEvent *event);

    // Menü

private slots:
    // Erstellen von Fahrtagen und Aufruf zum Bearbeiten
    void on_calendarDock_activated(const QDate &date);

    void on_calendarDock_clicked(const QDate &date);
    void on_pushListeShowAll_clicked();

    void on_ListeZuege_itemSelectionChanged();
    void on_ListeZuege_itemClicked(QListWidgetItem *item);

    void on_ButtonAdd_clicked();
    void on_ButtonRemove_clicked();

    void on_ButtonVorher_clicked();
    void on_ButtonNachher_clicked();

    // Verändern der Rahmendaten

    void on_dateZug_dateChanged(const QDate &date);
    void on_comboArt_currentIndexChanged(int index);
    void on_textAnlass_textChanged();
    void on_checkWichtig_stateChanged(int arg1);
    void on_comboWagenreihung_currentTextChanged(QString string);
    void on_comboTimeTfH_currentIndexChanged(int index);
    void on_comboTimeTfM_currentTextChanged(QString string);
    void on_comboTimeZH_currentIndexChanged(int index);
    void on_comboTimeZM_currentTextChanged(QString string);

    // Verändern des Fahrplans

    // Verändern des Personals

    void on_pushButtonListTfAdd_clicked();
    void on_pushButtonListZfAdd_clicked();
    void on_pushButtonListZubAdd_clicked();
    void on_pushButtonListServiceAdd_clicked();

    void on_pushButtonListTfDelete_clicked();
    void on_pushButtonListZfDelete_clicked();
    void on_pushButtonListZubDelete_clicked();
    void on_pushButtonListServiceDelete_clicked();

    void on_checkTf_stateChanged(int arg1);
    void on_checkZf_stateChanged(int arg1);
    void on_checkZub_stateChanged(int arg1);
    void on_checkService_stateChanged(int arg1);

    void on_listTf_itemChanged(QListWidgetItem *item);
    void on_listZf_itemChanged(QListWidgetItem *item);
    void on_listZub_itemChanged(QListWidgetItem *item);
    void on_listService_itemChanged(QListWidgetItem *item);

    // Verändern der Reservierungen

    void on_toolResAdd_clicked();
    void on_toolResDelete_clicked();

    void on_listRes_itemSelectionChanged();
    void on_listRes_itemChanged(QListWidgetItem *item);

    void on_pushResAllAuto_clicked();
    void on_pushResVerteilung_clicked();

    void on_lineResName_textChanged(const QString &arg1);
    void on_spinResAnzahl_valueChanged(int arg1);
    void on_comboResKlasse_currentIndexChanged(int index);
    void on_checkResFahrrad_stateChanged(int arg1);
    void on_lineResMail_textChanged(const QString &arg1);
    void on_lineResTelefon_textChanged(const QString &arg1);
    void on_checkResAuto_stateChanged(int arg1);
    void on_lineResSitze_textChanged(const QString &arg1);
    void on_plainResSonstiges_textChanged();
    void on_comboResStart1Zug_currentIndexChanged(int index);
    void on_comboResStart1Hp_currentIndexChanged(int index);
    void on_comboResEnde1Zug_currentIndexChanged(int index);
    void on_comboResEnde1Hp_currentIndexChanged(int index);

    // Ein-/Ausgabe
    void on_ButtonExport_clicked();

    // Menü
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_actionSpeichernUnter_triggered();
    void on_actionSpeichern_triggered();
    void on_actionOeffnen_triggered();
    void on_actionDrucken_triggered();

    // Hilfsmethoden

};

#endif // PLANERFAHRTAGE_H
