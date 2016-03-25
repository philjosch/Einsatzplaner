#ifndef PLANERFAHRTAGE_H
#define PLANERFAHRTAGE_H
#include <QListWidgetItem>
#include <QMainWindow>
#include <QListWidgetItem>
#include <manager.h>

namespace Ui {
class PlanerFahrtage;
}

class PlanerFahrtage : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlanerFahrtage(QWidget *parent = 0);
    ~PlanerFahrtage();
private:
    Ui::PlanerFahrtage *ui;
    bool saved;
    QString path;
    bool uebernehmen;
    QListWidgetItem *aktuellerZug;
    Manager *fahrplanManager;
    QList<QColor> Farben;

private slots:
    // Events
    void closeEvent(QCloseEvent *event);

    void on_ListeZuege_itemClicked(QListWidgetItem *item);
    void setState(bool aktiv);
    void on_ButtonAdd_clicked();
    void on_ButtonRemove_clicked();

    void createEmptyForm();
//    void bearbeiten(QListWidgetItem *zug);
    void aktualisieren();
    void ZugLaden(QListWidgetItem *zug);
    void ZugSichern();
    void deleteSelectedRowFromListWidget(QListWidget* curr);
    void addEmptyRowToListWidget(QListWidget* curr);

    void closeEinsatzPlaner();


    // Elemente zu Liste hinzufügen oder löschen */

    /* Organisatorisches */
    void saveRahmendaten();

    void on_comboTimeTfH_currentIndexChanged(int index);
    void on_comboTimeTfM_currentTextChanged(QString string);
    void on_comboTimeZH_currentIndexChanged(int index);
    void on_comboTimeZM_currentTextChanged(QString string);
    void on_comboArt_currentIndexChanged(int index);
    void on_comboWagenreihung_currentTextChanged(QString string);
    void on_dateZug_dateChanged(const QDate &date);
    void on_checkWichtig_stateChanged(int arg1);
    void on_textAnlass_textChanged();

    /* Fahrplan */
    void saveFahrplan();

    /* Personal */
    void savePersonal();

    void on_toolButtonListTfDelete_clicked();
    void on_toolButtonListTfAdd_clicked();
    void on_toolButtonListZfDelete_clicked();
    void on_toolButtonListZfAdd_clicked();
    void on_toolButtonListZubDelete_clicked();
    void on_toolButtonListZubAdd_clicked();
    void on_toolButtonListServiceDelete_clicked();
    void on_toolButtonListServiceAdd_clicked();

    void on_listTf_itemChanged(QListWidgetItem *item);
    void on_listZf_itemChanged(QListWidgetItem *item);
    void on_listZub_itemChanged(QListWidgetItem *item);
    void on_listService_itemChanged(QListWidgetItem *item);

    /* Reservierungen */
    void saveReservierungen();

    void on_toolResAdd_clicked();
    void on_toolResDelete_clicked();

    void on_pushResAllAuto_clicked();
    void on_pushResVerteilung_clicked();

    void on_lineResName_textChanged(const QString &arg1);
    void on_spinResAnzahl_valueChanged(int arg1);
    void on_comboResKlasse_currentIndexChanged(int index);
    void on_checkResFahrrad_stateChanged(int arg1);
    void on_lineResMail_textChanged(const QString &arg1);
    void on_lineResTelefon_textChanged(const QString &arg1);
    void on_radioResAuto_clicked(bool checked);
    void on_radioResManu_clicked(bool checked);
    void on_lineResSitze_textChanged(const QString &arg1);
    void on_plainResSonstiges_textChanged();
    void on_comboResStart1Zug_currentIndexChanged(int index);
    void on_comboResStart1Hp_currentIndexChanged(int index);
    void on_comboResEnde1Zug_currentIndexChanged(int index);
    void on_comboResEnde1Hp_currentIndexChanged(int index);


    /* Menu-Actions */
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_actionSpeichernUnter_triggered();
    void on_actionSpeichern_triggered();

    /* File Actions */
    void speichern();
    void speichernUnter();



    void on_calendarDock_clicked(const QDate &date);
    void on_pushListeShowAll_clicked();
    void on_ListeZuege_itemSelectionChanged();
};

#endif // PLANERFAHRTAGE_H
