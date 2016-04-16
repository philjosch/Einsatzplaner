#include "ui_planerfahrtage.h"
#include "planerfahrtage.h"
#include "fahrtag.h"
#include <QTime>


void PlanerFahrtage::saveRahmendaten()
{
    if (uebernehmen)
    {
        aktuellerZug->setDatum(ui->dateZug->date());
        aktuellerZug->setArt(ui->comboArt->currentIndex());
        aktuellerZug->setAnlass(ui->textAnlass->toPlainText());
        aktuellerZug->setWichtig(ui->checkWichtig->isChecked());
        aktuellerZug->setWagenreihung(ui->comboWagenreihung->currentText());
        aktuellerZug->setTimeTf(QTime::fromString(ui->comboTimeTfH->currentText()+":"+ui->comboTimeTfM->currentText(), "hh:mm"));
        aktuellerZug->setTimeZ(QTime::fromString(ui->comboTimeZH->currentText()+":"+ui->comboTimeZM->currentText(), "hh:mm"));
        setSaved(false);

        aktuellerZug->getItem()->setBackgroundColor(getFarbe(aktuellerZug->getArt()));
        Fahrtag zug = *aktuellerZug;
        zugManager->updateFahrtag(aktuellerZug);
        ui->ListeZuege->setCurrentItem((&zug)->getItem()); // Diese Zeilen müssen so beibehalten werden!!
    }
}

void PlanerFahrtage::on_dateZug_dateChanged(const QDate &date) { saveRahmendaten(); }
void PlanerFahrtage::on_comboArt_currentIndexChanged(int index) {
    saveRahmendaten();
    switch (index)
    {
    case 1: limitReservierungen(1); break;
    case 4: limitReservierungen(2); break;
    case 5: limitReservierungen(0); break;
    case 6: limitReservierungen(0); break;
    default: limitReservierungen(-1);
    }
    // Die automatische Sitzplatzverteilung wird nur bei den Nikolauszügen unterstützt!!
    setAutoEnabled(index == 2);
}
void PlanerFahrtage::on_textAnlass_textChanged() { saveRahmendaten(); }
void PlanerFahrtage::on_checkWichtig_stateChanged(int arg1) { saveRahmendaten(); }
void PlanerFahrtage::on_comboWagenreihung_currentTextChanged(QString string) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeTfH_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeTfM_currentTextChanged(QString string) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeZH_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeZM_currentTextChanged(QString string) { saveRahmendaten(); }
