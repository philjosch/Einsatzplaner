#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"
#include <QMessageBox>
#include <QFileDialog>
#include "export.h"
#include "qfahrtag.h"
#include <QListWidgetItem>
#include <QDate>
#include <QTime>

PlanerFahrtage::PlanerFahrtage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlanerFahrtage)
{
    ui->setupUi(this);
    fahrplanManager = new Manager(ui->ListeZuege);
    path = "";
    saved = true;
    uebernehmen = false;
//    connect(ui->ListeZuege, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(bearbeiten(QListWidgetItem*)));

    Farben << QColor(255, 255, 255) << QColor(205,92,92) << QColor(238,130,238) << QColor(211, 211, 211) << QColor(154, 205, 50) << QColor(255, 165, 0) << QColor(135, 206, 250);

    setState(false);
}

PlanerFahrtage::~PlanerFahrtage()
{
    delete ui;
}
void PlanerFahrtage::closeEinsatzPlaner() {
    if (! saved) {
        int answ = QMessageBox::question(this, tr("Beenden?"), tr("Möchten Sie das Prgramm wirklich beenden? Die ungesicherten Information gehen verloren!"), QMessageBox::Save, QMessageBox::Close, QMessageBox::Cancel);
        if (answ == QMessageBox::Save) {
            speichern();
        } else if (answ == QMessageBox::Close) {
            saved = true;
        }
    }


    if (saved) {
        close();
    }

}

void PlanerFahrtage::closeEvent(QCloseEvent *event) {
    closeEinsatzPlaner();


    if (saved) {
        event->accept();
    } else {
        event->ignore();
    }
}


/* Fahrtag bearbeiten */
void PlanerFahrtage::createEmptyForm() {
    /* Alles leeren */

    // Rahmendaten
    ui->dateZug->setDate(QDate::currentDate());
    ui->comboArt->setCurrentIndex(-1);
    ui->textAnlass->clear();
    ui->checkWichtig->setChecked(false);
    ui->comboWagenreihung->setCurrentIndex(-1);
    ui->comboTimeTfH->setCurrentIndex(3);
    ui->comboTimeTfM->setCurrentIndex(0);
    ui->comboTimeZH->setCurrentIndex(3);
    ui->comboTimeZM->setCurrentIndex(0);

    // Fahrplan

    // Personal
    ui->listTf->clear();
    ui->listZf->clear();
    ui->listZub->clear();
    ui->listService->clear();
    ui->textBemerkungen->clear();

    // Reservierungen
    ui->listRes->clear();
    ui->lineResName->clear();
    ui->spinResAnzahl->setValue(1);
    ui->checkResFahrrad->setChecked(false);
    ui->lineResMail->clear();
    ui->lineResTelefon->clear();
    ui->comboResKlasse->setCurrentIndex(3);
    ui->comboResStart1Zug->setCurrentIndex(1);
    ui->comboResStart1Hp->setCurrentIndex(10);
    ui->comboResEnde1Zug->setCurrentIndex(2);
    ui->comboResEnde1Hp->setCurrentIndex(10);
    ui->lineResSitze->clear();
    ui->plainResSonstiges->clear();

    /* Mit Standard-Werten füllen */
    addEmptyRowToListWidget(ui->listTf);
    addEmptyRowToListWidget(ui->listZf);
    addEmptyRowToListWidget(ui->listZub);
    addEmptyRowToListWidget(ui->listService);
}

void PlanerFahrtage::ZugLaden(QListWidgetItem* zug) {
    aktuellerZug = zug;
    createEmptyForm();
    QFahrtag *akt = fahrplanManager->getFahrtag(zug);
    // Rahmendaten
    ui->dateZug->setDate(akt->getDatum());
    ui->textAnlass->insertPlainText(akt->getAnlass());
    ui->comboArt->setCurrentIndex(akt->getArt());
    ui->checkWichtig->setChecked(akt->getWichtig());
    ui->comboWagenreihung->setCurrentText(akt->getWagenreihung());
    ui->comboTimeTfH->setCurrentText(akt->getTimeTf().toString("hh"));
    ui->comboTimeTfM->setCurrentText(akt->getTimeTf().toString("mm"));
    ui->comboTimeZH->setCurrentText(akt->getTimeZ().toString("hh"));
    ui->comboTimeZM->setCurrentText(akt->getTimeZ().toString("mm"));
    // Fahrplan
    // Peronal
    if (akt->getTf()->size() > 0) {
        ui->listTf->clear();
        ui->listTf->addItems(QStringList(*akt->getTf()));
        for(int i = 0; i < ui->listTf->count(); i++) {
            ui->listTf->item(i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        }
    }
    if (akt->getZf()->size() > 0) {
        ui->listZf->clear();
        ui->listZf->addItems(QStringList(*akt->getZf()));
        for(int i = 0; i < ui->listTf->count(); i++) {
            ui->listZf->item(i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        }
    }
    if (akt->getZub()->size() > 0) {
        ui->listZub->clear();
        ui->listZub->addItems(QStringList(*akt->getZub()));
        for(int i = 0; i < ui->listTf->count(); i++) {
            ui->listZub->item(i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        }
    }
    if (akt->getService()->size() > 0) {
        ui->listService->clear();
        ui->listService->addItems(QStringList(*akt->getService()));
        for(int i = 0; i < ui->listTf->count(); i++) {
            ui->listService->item(i)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        }
    }

    // Reservierungen
    uebernehmen = true;
    aktualisieren();
}


/* Sichern der Daten eines Fahrtages */
void PlanerFahrtage::saveRahmendaten() {
    if (uebernehmen) {
        QFahrtag *akt = fahrplanManager->getFahrtag(aktuellerZug);
        akt->setDatum(ui->dateZug->date());
        akt->setArt(ui->comboArt->currentIndex());
        akt->setAnlass(ui->textAnlass->toPlainText());
        akt->setWichtig(ui->checkWichtig->isChecked());
        akt->setWagenreihung(ui->comboWagenreihung->currentText());
        akt->setTimeTf(QTime::fromString(ui->comboTimeTfH->currentText()+":"+ui->comboTimeTfM->currentText(), "hh:mm"));
        akt->setTimeZ(QTime::fromString(ui->comboTimeZH->currentText()+":"+ui->comboTimeZM->currentText(), "hh:mm"));
        saved = false;

        QListWidgetItem *item = akt->getListItem();
        item->setText(akt->getDatum().toString("dddd d. M. yyyy") + (akt->getWichtig() ? " "+tr("WICHTIG!") : ""));
        item->setBackgroundColor(Farben.at(akt->getArt()));
        fahrplanManager->updateFahrtag(akt);
        ui->ListeZuege->setCurrentItem(item);
//        aktualisieren();
    }
}

void PlanerFahrtage::saveFahrplan() {
    if (uebernehmen) {
        saved = false;
//        aktualisieren();
    }
}

void PlanerFahrtage::savePersonal() {
    if (uebernehmen) {
        QFahrtag *akt = fahrplanManager->getFahrtag(aktuellerZug);
        // Liste mit Peronal Speichern
        QList<QString>* tf = new QList<QString>();
        for(int i = 0; i < ui->listTf->count(); i++) {
            if (ui->listTf->item(i)->text() != tr("(Name eingeben)"))
                tf->append(ui->listTf->item(i)->text());
        }
        akt->setTf(tf);
        QList<QString>* zf = new QList<QString>();
        for(int i = 0; i < ui->listZf->count(); i++) {
            if (ui->listZf->item(i)->text() != tr("(Name eingeben)"))
                zf->append(ui->listZf->item(i)->text());
        }
        akt->setZf(zf);
        QList<QString>* zub = new QList<QString>();
        for(int i = 0; i < ui->listZub->count(); i++) {
            if (ui->listZub->item(i)->text() != tr("(Name eingeben)"))
                zub->append(ui->listZub->item(i)->text());
        }
        akt->setZub(zub);
        QList<QString>* service = new QList<QString>();
        for(int i = 0; i < ui->listService->count(); i++) {
            if (ui->listService->item(i)->text() != tr("(Name eingeben)"))
                service->append(ui->listService->item(i)->text());
        }
        akt->setService(service);

        akt->setBemerkungen(ui->textBemerkungen->toPlainText());
        saved = false;
//        aktualisieren();
    }
}

void PlanerFahrtage::saveReservierungen() {
    if (uebernehmen) {
//        QFahrtag *akt = fahrplanManager->getFahrtag(aktuellerZug);
        saved = false;
        aktualisieren();
    }
}

void PlanerFahrtage::ZugSichern() {
    if (uebernehmen) {
        saveRahmendaten();
        saveFahrplan();
        savePersonal();
        saveReservierungen();
    }
}

void PlanerFahrtage::aktualisieren() { }


/* Speichern einer Datei */
void PlanerFahrtage::speichern() {
    if (path == "") {
        speichernUnter();
    }
    if (path != "") {
        ZugSichern();
        // Alle Daten in JSON umwandeln und dann in die Datei schreiben
        saved = true;
    }
}

void PlanerFahrtage::speichernUnter() {
    path = QFileDialog::getSaveFileName(this, tr("Speichern unter..."), "Einsatzplan.ako", tr("AkO-Dateien (*.ako)"));
    setWindowFilePath(path);
    if (path != "") {
        speichern();
    }
}



void PlanerFahrtage::setState(bool aktiv) {
    /* Rahmendaten */
    ui->dateZug->setEnabled(aktiv);
    ui->comboArt->setEnabled(aktiv);
    ui->comboWagenreihung->setEnabled(aktiv);
    ui->checkWichtig->setEnabled(aktiv);
    ui->textAnlass->setEnabled(aktiv);
    ui->textBemerkungen->setEnabled(aktiv);
    ui->comboTimeTfH->setEnabled(aktiv);
    ui->comboTimeTfM->setEnabled(aktiv);
    ui->comboTimeZH->setEnabled(aktiv);
    ui->comboTimeZM->setEnabled(aktiv);
    /* Fahrplan */
//    ui->tabFahrplan->setEnabled(aktiv);
    /* Personal */
    ui->tabPersonal->setEnabled(aktiv);
    /* Reservierungen */
    ui->tabReservierungen->setEnabled(aktiv);
}

/* Methoden, um Listen zu manipulieren */
void PlanerFahrtage::deleteSelectedRowFromListWidget(QListWidget* curr)
{
    if (curr->count() > 1) {
        curr->takeItem(ui->listTf->currentRow());
    }
}
void PlanerFahrtage::addEmptyRowToListWidget(QListWidget* curr)
{
    curr->addItem("(Name eingeben)");
    curr->item(curr->count()-1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
}


