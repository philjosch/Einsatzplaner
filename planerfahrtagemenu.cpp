#include "planerfahrtage.h"
#include <QMessageBox>
#include <QListWidgetItem>


void PlanerFahrtage::on_actionQuit_triggered() {
    closeEinsatzPlaner();
}

void PlanerFahrtage::on_actionAbout_triggered() {
    QMessageBox::information(this, "Info", "Keine Informationen zu Einsatzplaner gefunden!", QMessageBox::Abort);
}
void PlanerFahrtage::on_actionPreferences_triggered() {
    QMessageBox::information(this, "Info", "Es gibt noch keine Einstellungen!", QMessageBox::Abort);
}
void PlanerFahrtage::on_actionSpeichernUnter_triggered() {
    speichernUnter();
}
void PlanerFahrtage::on_actionSpeichern_triggered() {
    speichern();
}
