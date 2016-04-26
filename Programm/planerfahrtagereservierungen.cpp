#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"
#include <QMessageBox>
#include <QFileDialog>
#include "export.h"
#include <QListWidgetItem>
#include <QDate>
#include <QTime>


void PlanerFahrtage::saveReservierungen()
{
    if (uebernehmen)
    {
        setSaved(false);
        Reservierung *res = aktuellerZug->getManager()->getCurrentReservierung();
        res->setName(ui->lineResName->text());
        res->setAnzahl(ui->spinResAnzahl->value());
        res->setKlasse(ui->comboResKlasse->currentText());
        res->setFahrrad(ui->checkResFahrrad->isChecked());
        res->setMail(ui->lineResMail->text());
        res->setTelefon(ui->lineResTelefon->text());
        res->setStartZug(ui->comboResStart1Zug->currentText(), 0);
        res->setStartHp(ui->comboResStart1Hp->currentText(), 0);
        res->setEndeZug(ui->comboResEnde1Zug->currentText(), 0);
        res->setEndeHp(ui->comboResEnde1Hp->currentText(), 0);
        res->setAutoPlatz(ui->checkResAuto->isChecked());
        res->setSitzplatz(ui->lineResSitze->text());
        res->setSonstiges(ui->plainResSonstiges->toPlainText());
    }
}

// Inzufügen von Reservierungen und löschen
void PlanerFahrtage::on_toolResAdd_clicked()
{
//    saveReservierungen();
    aktuellerZug->getManager()->add();
    ui->toolResDelete->setEnabled(true);
    ui->lineResName->setFocus();
}
void PlanerFahrtage::on_toolResDelete_clicked()
{
    // Abfrage, ob Löschen wirklich erwünscht ist
    aktuellerZug->getManager()->remove(ui->listRes->currentItem());
    if (ui->listRes->count() < 1) {
        ui->toolResDelete->setEnabled(false);
        setStateRes(false);
    }
}

/* Aktionen */
void PlanerFahrtage::on_listRes_itemSelectionChanged()
{
    if (uebernehmen)
    {
        uebernehmen = false;
        // Reservierungen
        setStateRes(true);

        Reservierung *res = aktuellerZug->getManager()->getCurrentReservierung();
        ui->lineResName->setText(res->getName());
        ui->spinResAnzahl->setValue(res->getAnzahl());
        ui->comboResKlasse->setCurrentText(res->getKlasse());
        ui->checkResFahrrad->setChecked(res->getFahrrad());
        ui->lineResMail->setText(res->getMail());
        ui->lineResTelefon->setText(res->getTelefon());
        ui->comboResStart1Zug->setCurrentText(res->getStartZug().at(0));
        ui->comboResStart1Hp->setCurrentText(res->getStartHp().at(0));
        ui->comboResEnde1Zug->setCurrentText(res->getEndeZug().at(0));
        ui->comboResEnde1Hp->setCurrentText(res->getEndeHp().at(0));
        ui->checkResAuto->setChecked(res->getAutoPlatz());
        ui->lineResSitze->setEnabled(! res->getAutoPlatz());
        ui->lineResSitze->setText(res->getSitzplatz());
        ui->plainResSonstiges->clear();
        ui->plainResSonstiges->insertPlainText(res->getSonstiges());

        uebernehmen = true;
    }
}
// Write-Throu für Name der Reservierenden Person
void PlanerFahrtage::on_listRes_itemChanged(QListWidgetItem *item)
{
    ui->listRes->setCurrentItem(item);
    ui->lineResName->setText(item->text());
}

// Atmatische Verteilung der Sitzplätze (fehlt)
void PlanerFahrtage::on_pushResAllAuto_clicked()
{
}
// Anzeigen einer Sitzplatzverteilung,anhand der Wagenreihung, ... (fehlt)
void PlanerFahrtage::on_pushResVerteilung_clicked()
{
}

/* Änderungen bei der Eingabe direkt speichern */
void PlanerFahrtage::on_lineResName_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_spinResAnzahl_valueChanged(int arg1) { saveReservierungen(); }
void PlanerFahrtage::on_comboResKlasse_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_checkResFahrrad_stateChanged(int arg1) { saveReservierungen(); }
void PlanerFahrtage::on_lineResMail_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_lineResTelefon_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_comboResStart1Zug_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_comboResStart1Hp_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_comboResEnde1Zug_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_comboResEnde1Hp_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_checkResAuto_stateChanged(int arg1)
{
    ui->lineResSitze->setEnabled(! ui->checkResAuto->isChecked());
    saveReservierungen();
}
void PlanerFahrtage::on_lineResSitze_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_plainResSonstiges_textChanged() { saveReservierungen(); }

