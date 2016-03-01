#include "planerfahrtage.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
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
    saved = false;
    uebernehmen = false;
//    connect(ui->ListeZuege, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(bearbeiten(QListWidgetItem*)));
//    connect(ui->TextAnlass, SIGNAL(textChanged(QString)), this, SLOT(unSichern()));

    Farben << QColor(255, 255, 255) << QColor(205,92,92) << QColor(238,130,238) << QColor(211, 211, 211) << QColor(154, 205, 50) << QColor(255, 165, 0) << QColor(135, 206, 250);

}

PlanerFahrtage::~PlanerFahrtage()
{
    delete ui;
}

void PlanerFahrtage::createEmptyForm() {
    // Listen und Formulare leeren
    ui->dateZug->setDate(QDate::currentDate());
    ui->comboArt->setCurrentIndex(-1);
    ui->textAnlass->clear();
    ui->checkWichtig->setChecked(false);
    ui->comboWagenreihung->setCurrentIndex(-1);
    ui->comboTimeTfH->setCurrentIndex(3);
    ui->comboTimeTfM->setCurrentIndex(0);
    ui->comboTimeZH->setCurrentIndex(3);
    ui->comboTimeZM->setCurrentIndex(0);
    ui->listTf->clear();
    ui->listZf->clear();
    ui->listZub->clear();
    ui->listService->clear();


    // Listen und Formulare mit Standard Werten füllen
    addEmptyRowToListWidget(ui->listTf);
    addEmptyRowToListWidget(ui->listZf);
    addEmptyRowToListWidget(ui->listZub);
    addEmptyRowToListWidget(ui->listService);
    on_toolButtonTableReservierungenAdd_clicked();

}

void PlanerFahrtage::on_ButtonAdd_clicked ()
{
    QFahrtag *neu_Fahrtag = new QFahrtag();
    QListWidgetItem *neu_Feld = new QListWidgetItem();
    neu_Feld->setText(neu_Fahrtag->getDatum().toString("dddd d. M. yyyy"));
    ui->ListeZuege->addItem(neu_Feld);
    Fahrtage.insert(neu_Feld, neu_Fahrtag);

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
    createEmptyForm();
    ZugLaden(item);
    uebernehmen = true;

}

void PlanerFahrtage::ZugLaden(QListWidgetItem* zug) {
    aktuellerZug = zug;

    QFahrtag *akt = Fahrtage[zug];

    ui->dateZug->setDate(akt->getDatum());
    ui->textAnlass->clear();
    ui->textAnlass->insertPlainText(akt->getAnlass());
    ui->comboArt->setCurrentIndex(akt->getArt());
    ui->checkWichtig->setChecked(akt->getWichtig());
    ui->comboWagenreihung->setCurrentText(akt->getWagenreihung());
    ui->comboTimeTfH->setCurrentText(akt->getTimeTf().toString("hh"));
    ui->comboTimeTfM->setCurrentText(akt->getTimeTf().toString("mm"));
    ui->comboTimeZH->setCurrentText(akt->getTimeZ().toString("hh"));
    ui->comboTimeZM->setCurrentText(akt->getTimeZ().toString("mm"));


    if (akt->getTf()->size() > 0) {
        ui->listTf->clear();
        ui->listTf->addItems(QStringList(*akt->getTf()));
    }
    if (akt->getZf()->size() > 0) {
        ui->listZf->clear();
        ui->listZf->addItems(QStringList(*akt->getZf()));
    }
    if (akt->getZub()->size() > 0) {
        ui->listZub->clear();
        ui->listZub->addItems(QStringList(*akt->getZub()));
    }
    if (akt->getService()->size() > 0) {
        ui->listService->clear();
        ui->listService->addItems(QStringList(*akt->getService()));
    }
    uebernehmen = true;
    aktualisieren();
}

void PlanerFahrtage::ZugSichern()
{
    if (uebernehmen) {
    QFahrtag *akt = Fahrtage[aktuellerZug];

    akt->setDatum(ui->dateZug->date());
    akt->setArt(ui->comboArt->currentIndex());
    akt->setAnlass(ui->textAnlass->toPlainText());
    akt->setWichtig(ui->checkWichtig->isChecked());
    akt->setWagenreihung(ui->comboWagenreihung->currentText());
    akt->setTimeTf(QTime::fromString(ui->comboTimeTfH->currentText()+":"+ui->comboTimeTfM->currentText(), "hh:mm"));
    akt->setTimeZ(QTime::fromString(ui->comboTimeZH->currentText()+":"+ui->comboTimeZM->currentText(), "hh:mm"));

    QList<QString>* tf = new QList<QString>();
    for(int i = 0; i < ui->listTf->count(); i++) {
        tf->append(ui->listTf->item(i)->text());
    }
    akt->setTf(tf);
    QList<QString>* zf = new QList<QString>();
    for(int i = 0; i < ui->listZf->count(); i++) {
        zf->append(ui->listZf->item(i)->text());
    }
    akt->setZf(zf);
    QList<QString>* zub = new QList<QString>();
    for(int i = 0; i < ui->listZub->count(); i++) {
        zub->append(ui->listZub->item(i)->text());
    }
    akt->setZub(zub);
    QList<QString>* service = new QList<QString>();
    for(int i = 0; i < ui->listService->count(); i++) {
        service->append(ui->listService->item(i)->text());
    }
    akt->setService(service);

    aktualisieren();

    }
}

void PlanerFahrtage::aktualisieren()
{

    if (uebernehmen) {
    foreach(QListWidgetItem *zug, Fahrtage.keys()) {

        QFahrtag *akt = Fahrtage[zug];
        zug->setText(akt->getDatum().toString("dddd d. M. yyyy"));
        zug->setBackgroundColor(Farben.at(akt->getArt()));

    }
    ui->ListeZuege->sortItems();
    }

}

void PlanerFahrtage::setState(bool aktiv)
{
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
    }
}
void PlanerFahrtage::on_toolButtonTableReservierungenAdd_clicked() { ui->tableReservierungen->insertRow(0); }

void PlanerFahrtage::changeSizeOfTableReservierungen() {
    int width = ui->tableReservierungen->size().width();
    ui->tableReservierungen->setColumnWidth(0, width * .22);
    ui->tableReservierungen->setColumnWidth(1, width * .22);
    ui->tableReservierungen->setColumnWidth(2, width * .22);
    ui->tableReservierungen->setColumnWidth(3, width * .22);
}
void PlanerFahrtage::resizeEvent(QResizeEvent *event) { changeSizeOfTableReservierungen(); }

void PlanerFahrtage::on_comboTimeTfH_currentIndexChanged(int index) { ZugSichern(); }
void PlanerFahrtage::on_comboTimeTfM_currentTextChanged(QString string) { ZugSichern(); }
void PlanerFahrtage::on_comboTimeZH_currentIndexChanged(int index) { ZugSichern(); }
void PlanerFahrtage::on_comboTimeZM_currentTextChanged(QString string) { ZugSichern(); }
void PlanerFahrtage::on_comboArt_currentIndexChanged(int index) { ZugSichern(); }
void PlanerFahrtage::on_comboWagenreihung_currentTextChanged(QString string) { ZugSichern(); }

void PlanerFahrtage::on_dateZug_dateChanged(const QDate &date) { ZugSichern(); }

void PlanerFahrtage::on_checkWichtig_stateChanged(int arg1) { ZugSichern(); }

void PlanerFahrtage::on_textAnlass_textChanged() { ZugSichern(); }

void PlanerFahrtage::on_listTf_itemChanged(QListWidgetItem *item) { ZugSichern(); }
void PlanerFahrtage::on_listZf_itemChanged(QListWidgetItem *item) { ZugSichern(); }
void PlanerFahrtage::on_listZub_itemChanged(QListWidgetItem *item) { ZugSichern(); }
void PlanerFahrtage::on_listService_itemChanged(QListWidgetItem *item) { ZugSichern(); }

void PlanerFahrtage::on_tableReservierungen_cellChanged(int row, int column) { ZugSichern(); }





