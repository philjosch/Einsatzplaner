#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"
#include <QMessageBox>
#include "fahrtag.h"
#include <QListWidgetItem>
#include <QDate>
#include <QTime>
#include <QList>

PlanerFahrtage::PlanerFahrtage(QWidget *parent) : QMainWindow(parent), ui(new Ui::PlanerFahrtage)
{
    ui->setupUi(this);
    zugManager = new ManagerZuege(ui->ListeZuege, ui->listRes);
    path = "";
    saved = true;
    uebernehmen = false;
    setState(false);
    createEmptyForm();
    ui->tabEingabeWizard->removeTab(0); // Löscht den Fahrplan-Tab, da dieser im Moment nicht benötigt wird
}

PlanerFahrtage::~PlanerFahrtage()
{
    delete ui;
}

QString PlanerFahrtage::getArt(int i)
{
    switch (i) {
        case 0: return "Museumszug";
        case 1: return "Sonderzug";
        case 2: return "Nikolauszug";
        case 3: return "Museumszug mit ELF";
        case 4: return "ELF-Schnupperkurs";
        case 5: return "Bahnhofsfest";
        case 6: return "Ausbildung";
        default:
            return "Sonstiges";
    }
}

QColor PlanerFahrtage::getFarbe(int i)
{
    switch (i) {
    case 0:     return "#ffffff"; // Museumszug
    case 1:     return "#ffc789"; // Sonderzug
    case 2:     return "#b7a0e3"; // Nikolausfahrt
    case 3:     return "#afcad6"; // Museumszug mit ELF
    case 4:     return "#99c9de"; // ELF-Schnupperkurs
    case 5:     return "#d3f7a7"; // Bahnhofsfest
    case 6:     return "#fff5ac"; // Ausbildung
    default:    return "#f2a4c3"; // Sonstiges
    }
}

/* ENDE DES BEREICHS MIT KONSTRUKTOR UND STATISCHEN METHODEN */

// Tag im Kalender aktiviert -> Zug Laden
void PlanerFahrtage::on_calendarDock_activated(const QDate &date)
{
    on_ButtonAdd_clicked();
}

// Anzeigen des gewählten Tages im Kalender
void PlanerFahrtage::on_calendarDock_clicked(const QDate &date)
{
    if (! zugManager->showDate(date))
    {
        ui->ListeZuege->clearSelection();
        setState(false);
    }
}
void PlanerFahrtage::on_pushListeShowAll_clicked()
{
    zugManager->showAll();
    ui->ListeZuege->setCurrentRow(0);
}

void PlanerFahrtage::on_ListeZuege_itemSelectionChanged()
{
    if (ui->ListeZuege->count() > 0)
        on_ListeZuege_itemClicked(ui->ListeZuege->currentItem());
}
void PlanerFahrtage::on_ListeZuege_itemClicked(QListWidgetItem *item)
{
    uebernehmen = false;
    setState(true);
    ZugSichern();
    aktuellerZug = NULL;
    if (item != NULL)
        ZugLaden(item);
    uebernehmen = true;
}

void PlanerFahrtage::on_ButtonAdd_clicked ()
{
    on_ListeZuege_itemClicked(zugManager->addFahrtag()->getItem());
    ui->dateZug->setDate(ui->calendarDock->selectedDate());
    ui->ButtonRemove->setEnabled(true);
    ui->ButtonExport->setEnabled(true);
    ui->comboArt->setFocus();
    setSaved(false);
}
void PlanerFahrtage::on_ButtonRemove_clicked()
{
    if (QMessageBox::question(this, "Bitte bestätigen!", "Möchten Sie den Fahrtag wirklich löschen?") == QMessageBox::Yes)
    {
        zugManager->removeFahrtag(aktuellerZug);
        setSaved(false);
    }
    if (ui->ListeZuege->count() < 1)
    {
        setState(false);
        ui->ButtonNachher->setEnabled(false);
        ui->ButtonVorher->setEnabled(false);
        ui->ButtonRemove->setEnabled(false);
        ui->ButtonExport->setEnabled(false);
    }
}

void PlanerFahrtage::ZugLaden(QListWidgetItem* zug)
{
    aktuellerZug = zugManager->getFahrtag(zug);
    createEmptyForm();
    // Rahmendaten
    ui->dateZug->setDate(aktuellerZug->getDatum());
    ui->textAnlass->insertPlainText(aktuellerZug->getAnlass());
    ui->comboArt->setCurrentIndex(aktuellerZug->getArt());
    ui->checkWichtig->setChecked(aktuellerZug->getWichtig());
    ui->comboWagenreihung->setCurrentText(aktuellerZug->getWagenreihung());
    ui->comboTimeTfH->setCurrentText(aktuellerZug->getTimeTf().toString("hh"));
    ui->comboTimeTfM->setCurrentText(aktuellerZug->getTimeTf().toString("mm"));
    ui->comboTimeZH->setCurrentText(aktuellerZug->getTimeZ().toString("hh"));
    ui->comboTimeZM->setCurrentText(aktuellerZug->getTimeZ().toString("mm"));

    /* Personal */
    // CheckBoxes, ob noch Personal benötigt wird
    ui->checkTf->setChecked(aktuellerZug->getBenoetigeTf());
    ui->checkZf->setChecked(aktuellerZug->getBenoetigeZf());
    ui->checkZub->setChecked(aktuellerZug->getBenoetigeZub());
    ui->checkService->setChecked(aktuellerZug->getBenoetigeService());
    // Laden der einzelnen Listenelemente
    qListToListWidget(aktuellerZug->getTf(), ui->listTf);
    qListToListWidget(aktuellerZug->getZf(), ui->listZf);
    qListToListWidget(aktuellerZug->getZub(), ui->listZub);
    qListToListWidget(aktuellerZug->getService(), ui->listService);
    // Löschen-Buttons aktivieren, wenn nötig
    ui->pushButtonListTfDelete->setEnabled(aktuellerZug->getTf()->length() > 0);
    ui->pushButtonListZfDelete->setEnabled(aktuellerZug->getZf()->length() > 0);
    ui->pushButtonListZubDelete->setEnabled(aktuellerZug->getZub()->length() > 0);
    ui->pushButtonListServiceDelete->setEnabled(aktuellerZug->getService()->length() > 0);
    // Einsetzen der Bemerkungen
    ui->textBemerkungen->insertPlainText(aktuellerZug->getBemerkungen());

    /* Reservierungen */
    setStateRes(false); // Eingabemaske für Reservierungen deaktivieren
    aktuellerZug->getManager()->catching(); // Die Daten vom Manager in die Liste laden
    ui->toolResDelete->setEnabled(aktuellerZug->getManager()->getGesamtzahl() > 0);

    /* Sonstige Anpassungen */
    // Den Fokus auf das Feld mit dem Anlass legen
    ui->textAnlass->setFocus();
    // Buttons für weiter und zurück aktivieren/Deaktivieren
    ui->ButtonVorher->setEnabled(ui->ListeZuege->currentRow() > 0);
    ui->ButtonNachher->setEnabled(ui->ListeZuege->currentRow()+1 < ui->ListeZuege->count());

    uebernehmen = true;
}

void PlanerFahrtage::ZugSichern()
{
    if (uebernehmen)
    {
        saveRahmendaten();
        saveFahrplan();
        savePersonal();
        saveReservierungen();
    }
}

// Vorherigen Fahrtag auswählen
void PlanerFahrtage::on_ButtonVorher_clicked()
{
    zugManager->showAll();
    ui->ListeZuege->setCurrentRow(ui->ListeZuege->currentRow()-1);
}
// Nächsten Fahrtag auswählen
void PlanerFahrtage::on_ButtonNachher_clicked()
{
    zugManager->showAll();
    ui->ListeZuege->setCurrentRow(ui->ListeZuege->currentRow()+1);
}
