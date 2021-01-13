#include "personwindow.h"
#include "ui_personwindow.h"

#include "export.h"

#include <QDesktopServices>
#include <QMessageBox>

PersonWindow::PersonWindow(QWidget *parent, Person *p) :
    QMainWindow(parent), ui(new Ui::PersonWindow)
{
    ui->setupUi(this);

    person = p;

    enabled = false;

    ui->lineID->setText(QString::number(p->getNummer()));
    ui->lineVorname->setText(p->getVorname());
    ui->lineNachname->setText(p->getNachname());

    // ** Stammdaten
    // Allgemein
    ui->checkGeburtstag->setChecked(p->getGeburtstag().isNull());
    ui->dateGeburtstag->setEnabled(p->getGeburtstag().isValid());
    ui->dateGeburtstag->setDate(p->getGeburtstag());
    ui->checkEintritt->setChecked(p->getEintritt().isNull());
    ui->dateEintritt->setEnabled(p->getEintritt().isValid());
    ui->dateEintritt->setDate(p->getEintritt());
    ui->checkAktiv->setChecked(p->getAktiv());
    ui->spinKm->setValue(p->getStrecke());
    ui->lineJob->setText(p->getBeruf());

    // Kontakt
    ui->lineStrasse->setText(p->getStrasse());
    ui->linePLZ->setText(p->getPLZ());
    ui->lineOrt->setText(p->getOrt());
    ui->linePhone->setText(p->getTelefon());
    ui->checkPhone->setChecked(p->getTelefonOK());
    ui->lineMail->setText(p->getMail());
    ui->checkMail->setChecked(p->getMailOK());

    // Betriebsdienst
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());

    ui->checkDienst->setChecked(p->getTauglichkeit().isNull());
    ui->dateDienst->setEnabled(p->getTauglichkeit().isValid());
    ui->dateDienst->setDate(p->getTauglichkeit());

    // Sonstiges
    ui->plainBemerkung->setPlainText(p->getBemerkungen().replace("<br/>","\n"));
    ui->checkAustritt->setChecked(p->getAustritt().isValid());
    ui->dateAustritt->setEnabled(p->getAustritt().isValid());
    ui->dateAustritt->setDate(p->getAustritt());

    enabled = true;

    setWindowTitle(person->getName());
}

PersonWindow::~PersonWindow()
{
    delete ui;
}

void PersonWindow::on_actionMail_triggered()
{
    if (person->getMail() != "") {
        QDesktopServices::openUrl(QUrl("mailto:"+person->getMail()));
    }
}

void PersonWindow::on_actionLoeschen_triggered()
{
    if (enabled) {
        if (person->getZeiten(Anzahl) > 0) {
            QMessageBox::information(this, tr("Warnung"), tr("Die ausgewählte Person kann nicht gelöscht werden, da Sie noch bei Aktivitäten eingetragen ist.\nBitte lösen Sie diese Verbindung bevor Sie die Person löschen!"));
            return;
        }
        if (QMessageBox::question(this, tr("Wirklich löschen"), tr("Möchten Sie die Person wirklich unwiderruflich löschen und aus dem System entfernen.\nFür ausgetretene Mitglieder können Sie auch ein Austrittsdatum angeben!")) != QMessageBox::Yes) {
            return;
        }
        enabled = false;

        emit person->del(person);
        this->close();
        deleteLater();
    }
}

void PersonWindow::on_actionEinzelPDF_triggered()
{
    Export::Mitglieder::printMitgliederEinzelEinzel(person,
                        Export::getPrinterPDF(this, "Stammdatenblatt.pdf", QPrinter::Orientation::Portrait));
}
void PersonWindow::on_actionEinzelDrucken_triggered()
{
    Export::Mitglieder::printMitgliederEinzelEinzel(person,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}


void PersonWindow::on_lineVorname_textChanged(const QString &arg1)
{
    if (enabled) {
        // test, ob Person vorhanden ist
        if (person->setVorname(arg1)) {
            setWindowTitle(person->getName());
            // Nichts zu tun
        } else {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        }
    }
}
void PersonWindow::on_lineNachname_textChanged(const QString &arg1)
{
    if (enabled) {
        if (person->setNachname(arg1)) {
            setWindowTitle(person->getName());
            // Nicht zu tun
        } else {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        }
    }
}

void PersonWindow::on_lineID_textChanged(const QString &arg1)
{
    if (enabled) {
        if (arg1.toInt() > 0) {
            if (! person->setNummer(arg1.toInt())) {
                QMessageBox::information(this, tr("Fehler"), tr("Die Mitgliedsnummer konnte nicht geändert werden, da sie bereits vergeben ist. Bitte wählen Sie eine andere Nummer!"));
            }
        }
    }
}
void PersonWindow::on_pushAutoID_clicked()
{
    if (enabled) {
        enabled=false;
        ui->lineID->setText(QString::number(person->setAutoNummer()));
        enabled=true;
    }
}

void PersonWindow::on_dateGeburtstag_dateChanged(const QDate &date)
{
    if (enabled) {
        person->setGeburtstag(date);
    }
}
void PersonWindow::on_checkGeburtstag_clicked(bool checked)
{
    if (enabled) {
        ui->dateGeburtstag->setEnabled(!checked);
        if (checked) {
            person->setGeburtstag(QDate());
        } else {
            ui->dateGeburtstag->setDate(QDate::currentDate());
            person->setGeburtstag(QDate::currentDate());
        }
    }
}

void PersonWindow::on_dateEintritt_dateChanged(const QDate &date)
{
    if (enabled) {
        person->setEintritt(date);
    }
}
void PersonWindow::on_checkEintritt_clicked(bool checked)
{
    if (enabled) {
        ui->dateEintritt->setEnabled(!checked);
        if (checked) {
            person->setEintritt(QDate());
        } else {
            ui->dateEintritt->setDate(QDate::currentDate());
            person->setEintritt(QDate::currentDate());
        }
    }
}

void PersonWindow::on_checkAktiv_clicked(bool checked)
{
    if (enabled) {
        person->setAktiv(checked);
    }
}

void PersonWindow::on_spinKm_valueChanged(int arg1)
{
    if (enabled) {
        person->setStrecke(arg1);
    }
}

void PersonWindow::on_lineJob_textChanged(const QString &arg1)
{
    if (enabled) {
        person->setBeruf(arg1);
    }
}

void PersonWindow::on_lineStrasse_textChanged(const QString &arg1)
{
    if (enabled) {
        person->setStrasse(arg1);
    }
}
void PersonWindow::on_linePLZ_textChanged(const QString &arg1)
{
    if (enabled) {
        person->setPLZ(arg1);
    }
}
void PersonWindow::on_lineOrt_textChanged(const QString &arg1)
{
    if (enabled) {
        person->setOrt(arg1);
    }
}

void PersonWindow::on_linePhone_textChanged(const QString &arg1)
{
    if (enabled) {
        person->setTelefon(arg1);
    }
}
void PersonWindow::on_checkPhone_clicked(bool checked)
{
    if (enabled) {
        person->setTelefonOK(checked);
    }
}

void PersonWindow::on_lineMail_textChanged(const QString &arg1)
{
    if (enabled) {
        person->setMail(arg1);
    }
}
void PersonWindow::on_checkMail_clicked(bool checked)
{
    if (enabled) {
        person->setMailOK(checked);
    }
}

void PersonWindow::on_checkTf_clicked(bool checked)
{
    if (enabled) {
        person->setAusbildungTf(checked);
    }
}
void PersonWindow::on_checkZf_clicked(bool checked)
{
    if (enabled) {
        person->setAusbildungZf(checked);
    }
}
void PersonWindow::on_checkRangierer_clicked(bool checked)
{
    if (enabled) {
        person->setAusbildungRangierer(checked);
    }
}

void PersonWindow::on_dateDienst_dateChanged(const QDate &date)
{
    if (enabled) {
        person->setTauglichkeit(date);
    }
}
void PersonWindow::on_checkDienst_clicked(bool checked)
{
    if (enabled) {
        ui->dateDienst->setEnabled(!checked);
        if (checked) {
            person->setTauglichkeit(QDate());
        } else {
            ui->dateDienst->setDate(QDate::currentDate());
            person->setTauglichkeit(QDate::currentDate());
        }
    }
}

void PersonWindow::on_plainBemerkung_textChanged()
{
    if (enabled) {
        person->setBemerkungen(ui->plainBemerkung->toPlainText());
    }
}

void PersonWindow::on_dateAustritt_dateChanged(const QDate &date)
{
    if (enabled) {
        person->setAustritt(date);
    }
}
void PersonWindow::on_checkAustritt_clicked(bool checked)
{
    if (enabled) {
        ui->dateAustritt->setEnabled(checked);
        if (checked) {
            ui->dateAustritt->setDate(QDate::currentDate());
            person->setAustritt(QDate::currentDate());
        } else {
            person->setAustritt(QDate());
        }
    }
}
