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

    changeSizeOfTableReservierungen();
    this->update();
}

PlanerFahrtage::~PlanerFahrtage()
{
    delete ui;
}

void PlanerFahrtage::on_actionQuit_triggered() {
    if (! saved) {
        int answ = QMessageBox::question(this, tr("Beenden?"), tr("Möchten Sie das Prgramm wirklich beenden? Die Ungesicherteren Information gehen verloren!"), QMessageBox::Close, QMessageBox::Save);
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
void PlanerFahrtage::speichern() {
    if (path == "") {
        speichernUnter();
    }
    if (path != "") {
        saved = true;
    }
}
void PlanerFahrtage::speichernUnter() {
    path = QFileDialog::getSaveFileName(this, tr("Speichern unter..."), "Einsatzplan.ako", tr("AkO-Dateien (*.ako)"));
}


void PlanerFahrtage::on_actionAbout_triggered() {
    QMessageBox::information(this, "Info", "Keine Informationen zu Einsatzplaner gefunden!", QMessageBox::Abort);
}
void PlanerFahrtage::on_actionPreferences_triggered() {
    QMessageBox::information(this, "Info", "Es gibt noch keine Einstellungen!", QMessageBox::Abort);
}


void PlanerFahrtage::createEmptyForm() {
    // Listen und Formulare leeren
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
    // ¡Empty!

    // Personal
    ui->listTf->clear();
    ui->listZf->clear();
    ui->listZub->clear();
    ui->listService->clear();
    ui->textBemerkungen->clear();

    // Reservierungen
    ui->tableReservierungen->setRowCount(0);


    // Listen und Formulare mit Standard Werten füllen
    addEmptyRowToListWidget(ui->listTf);
    addEmptyRowToListWidget(ui->listZf);
    addEmptyRowToListWidget(ui->listZub);
    addEmptyRowToListWidget(ui->listService);
    on_toolButtonTableReservierungenAdd_clicked();

}

void PlanerFahrtage::on_ButtonAdd_clicked ()
{

    QFahrtag *neuerFahrtag = new QFahrtag();
    QListWidgetItem *neuesFeld = new QListWidgetItem();
    neuesFeld->setText(neuerFahrtag->getDatum().toString("dddd d. M. yyyy"));
    ui->ListeZuege->addItem(neuesFeld);
//    Fahrtage.insert(neu_Feld, neu_Fahrtag);
    fahrplanManager->addFahrtag(neuerFahrtag, neuesFeld);

    saved = false;
    aktualisieren();
}
void PlanerFahrtage::on_ListeZuege_itemClicked(QListWidgetItem *item)
{
    on_ListeZuege_itemDoubleClicked(item);
}
void PlanerFahrtage::on_ListeZuege_itemDoubleClicked(QListWidgetItem *item)
{
    uebernehmen = false;
    setState(true);
    ZugSichern();
    aktuellerZug = NULL;
    ZugLaden(item);
    uebernehmen = true;

}

void PlanerFahrtage::ZugLaden(QListWidgetItem* zug) {
    aktuellerZug = zug;
//    QFahrtag *akt = Fahrtage[zug];
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
    // ¡Empty!

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
//    ui->tableReservierungen->
    uebernehmen = true;
    aktualisieren();
}


void PlanerFahrtage::saveRahmendaten() {
    if (uebernehmen) {
//        QFahrtag *akt = Fahrtage[aktuellerZug];
        QFahrtag *akt = fahrplanManager->getFahrtag(aktuellerZug);
        akt->setDatum(ui->dateZug->date());
        akt->setArt(ui->comboArt->currentIndex());
        akt->setAnlass(ui->textAnlass->toPlainText());
        akt->setWichtig(ui->checkWichtig->isChecked());
        akt->setWagenreihung(ui->comboWagenreihung->currentText());
        akt->setTimeTf(QTime::fromString(ui->comboTimeTfH->currentText()+":"+ui->comboTimeTfM->currentText(), "hh:mm"));
        akt->setTimeZ(QTime::fromString(ui->comboTimeZH->currentText()+":"+ui->comboTimeZM->currentText(), "hh:mm"));
        saved = false;

        QListWidgetItem *item = fahrplanManager->getListItem(akt);
        item->setText(akt->getDatum().toString("dddd d. M. yyyy") + (akt->getWichtig() ? " WICHTIG!" : ""));
        item->setBackgroundColor(Farben.at(akt->getArt()));
        fahrplanManager->updateFahrtag(akt);
        ui->ListeZuege->setCurrentItem(item);

//        aktualisieren();
    }
}

void PlanerFahrtage::saveFahrplan() {
    if (uebernehmen) {
        saved = false;
        aktualisieren();
    }
}

void PlanerFahrtage::savePersonal() {
    if (uebernehmen) {
//        QFahrtag *akt = Fahrtage[aktuellerZug];
        QFahrtag *akt = fahrplanManager->getFahrtag(aktuellerZug);
        // Liste mit Peronal Speichern
        QList<QString>* tf = new QList<QString>();
        for(int i = 0; i < ui->listTf->count(); i++) {
            if (ui->listTf->item(i)->text() != "(Name eingeben)")
                tf->append(ui->listTf->item(i)->text());
        }
        akt->setTf(tf);
        QList<QString>* zf = new QList<QString>();
        for(int i = 0; i < ui->listZf->count(); i++) {
            if (ui->listZf->item(i)->text() != "(Name eingeben)")
                zf->append(ui->listZf->item(i)->text());
        }
        akt->setZf(zf);
        QList<QString>* zub = new QList<QString>();
        for(int i = 0; i < ui->listZub->count(); i++) {
            if (ui->listZub->item(i)->text() != "(Name eingeben)")
                zub->append(ui->listZub->item(i)->text());
        }
        akt->setZub(zub);
        QList<QString>* service = new QList<QString>();
        for(int i = 0; i < ui->listService->count(); i++) {
            if (ui->listService->item(i)->text() != "(Name eingeben)")
                service->append(ui->listService->item(i)->text());
        }
        akt->setService(service);

        akt->setBemerkungen(ui->textBemerkungen->toPlainText());
        saved = false;
        aktualisieren();
    }
}

void PlanerFahrtage::saveReservierungen() {
    if (uebernehmen) {
//        QFahrtag *akt = Fahrtage[aktuellerZug];
        QFahrtag *akt = fahrplanManager->getFahrtag(aktuellerZug);
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

void PlanerFahrtage::setState(bool aktiv) {
    /* Rahmendaten */
    ui->dateZug->setEnabled(aktiv);
    ui->comboArt->setEnabled(aktiv);
    ui->comboWagenreihung->setEnabled(aktiv);
    ui->checkWichtig->setEnabled(aktiv);
    ui->textAnlass->setEnabled(aktiv);
    ui->textBemerkungen->setEnabled(aktiv);
    // Uhrzeit für Dienstbeginn
    ui->comboTimeTfH->setEnabled(aktiv);
    ui->comboTimeTfM->setEnabled(aktiv);
    ui->comboTimeZH->setEnabled(aktiv);
    ui->comboTimeZM->setEnabled(aktiv);

    /* Fahrplan */
    // ¡EMPTY!

    /* Personal */
    ui->listTf->setEnabled(aktiv);
    ui->listZf->setEnabled(aktiv);
    ui->listZub->setEnabled(aktiv);
    ui->listService->setEnabled(aktiv);
    ui->toolButtonListTfDelete->setEnabled(aktiv);
    ui->toolButtonListTfAdd->setEnabled(aktiv);
    ui->toolButtonListZfDelete->setEnabled(aktiv);
    ui->toolButtonListZfAdd->setEnabled(aktiv);
    ui->toolButtonListZubDelete->setEnabled(aktiv);
    ui->toolButtonListZubAdd->setEnabled(aktiv);
    ui->toolButtonListServiceDelete->setEnabled(aktiv);
    ui->toolButtonListServiceAdd->setEnabled(aktiv);


    /* Reservierungen */
    ui->tableReservierungen->setEnabled(aktiv);
    ui->toolButtonTableReservierungenDelete->setEnabled(aktiv);
    ui->toolButtonTableReservierungenAdd->setEnabled(aktiv);

}


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

void PlanerFahrtage::on_toolButtonListTfDelete_clicked() {
    deleteSelectedRowFromListWidget(ui->listTf);
}
void PlanerFahrtage::on_toolButtonListTfAdd_clicked() { addEmptyRowToListWidget(ui->listTf); }
void PlanerFahrtage::on_toolButtonListZfDelete_clicked() { deleteSelectedRowFromListWidget(ui->listZf); }
void PlanerFahrtage::on_toolButtonListZfAdd_clicked() { addEmptyRowToListWidget(ui->listZf); }
void PlanerFahrtage::on_toolButtonListZubDelete_clicked() { deleteSelectedRowFromListWidget(ui->listZub); }
void PlanerFahrtage::on_toolButtonListZubAdd_clicked() { addEmptyRowToListWidget(ui->listZub); }
void PlanerFahrtage::on_toolButtonListServiceDelete_clicked() { deleteSelectedRowFromListWidget(ui->listService); }
void PlanerFahrtage::on_toolButtonListServiceAdd_clicked() { addEmptyRowToListWidget(ui->listService); }


void PlanerFahrtage::on_toolButtonTableReservierungenDelete_clicked() {
    if (ui->tableReservierungen->rowCount() > 1) {
        ui->tableReservierungen->removeRow(ui->tableReservierungen->currentRow());
        update();
        changeSizeOfTableReservierungen();
    }
}
void PlanerFahrtage::on_toolButtonTableReservierungenAdd_clicked() { ui->tableReservierungen->insertRow(0); update(); changeSizeOfTableReservierungen();}

void PlanerFahrtage::changeSizeOfTableReservierungen() {
    int width = ui->tableReservierungen->size().width() - ui->tableReservierungen->verticalHeader()->width();
    ui->tableReservierungen->setColumnWidth(0, width * .16);
    ui->tableReservierungen->setColumnWidth(1, width * .16);
    ui->tableReservierungen->setColumnWidth(2, width * .16);
    ui->tableReservierungen->setColumnWidth(3, width * .16);
    ui->tableReservierungen->setColumnWidth(4, width * .16);
    ui->tableReservierungen->setColumnWidth(5, width * .16);
}
void PlanerFahrtage::resizeEvent(QResizeEvent *event) { changeSizeOfTableReservierungen(); }

// Sichern der Daten, wenn Eingaben verändert wurden
void PlanerFahrtage::on_dateZug_dateChanged(const QDate &date) { saveRahmendaten(); }
void PlanerFahrtage::on_comboArt_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_textAnlass_textChanged() { saveRahmendaten(); }
void PlanerFahrtage::on_checkWichtig_stateChanged(int arg1) { saveRahmendaten(); }
void PlanerFahrtage::on_comboWagenreihung_currentTextChanged(QString string) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeTfH_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeTfM_currentTextChanged(QString string) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeZH_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeZM_currentTextChanged(QString string) { saveRahmendaten(); }

// Fahrplan
//¡EMPTY!

// Personal
void PlanerFahrtage::on_listTf_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listZf_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listZub_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listService_itemChanged(QListWidgetItem *item) { savePersonal(); }

// Reservierungen
void PlanerFahrtage::on_tableReservierungen_cellChanged(int row, int column) { saveReservierungen(); }


