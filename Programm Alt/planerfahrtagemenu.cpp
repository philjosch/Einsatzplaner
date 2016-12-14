#include "planerfahrtage.h"
#include <QMessageBox>
#include <QListWidgetItem>


void PlanerFahrtage::on_actionQuit_triggered()
{
    closeEinsatzPlaner();
}

void PlanerFahrtage::on_actionAbout_triggered()
{
    QMessageBox::information(this, "Info", "Sie benutzen im Moment die erste Test-Verion von Einsatzplaner! Wenn Sie Fehler entdecken, melden Sie diese bitte den Entwicklern des Programms.", QMessageBox::Abort);
}


void PlanerFahrtage::on_actionPreferences_triggered()
{
    QMessageBox::information(this, "Einstellungen", "Im Moment werden noch keine Einstellungen unterstützt.", QMessageBox::Abort);
}

void PlanerFahrtage::on_actionSpeichernUnter_triggered()
{
    speichernUnter();
}

void PlanerFahrtage::on_actionSpeichern_triggered()
{
    speichern();
}

void PlanerFahrtage::on_actionOeffnen_triggered()
{
    oeffnen();
}

void PlanerFahrtage::on_actionDrucken_triggered()
{
    on_ButtonExport_clicked();
}
