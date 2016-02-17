#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "export.h"
#include "qfahrtag.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    saved = false;
    genommen = true;
    connect(ui->ListeZuege, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(bearbeiten(QListWidgetItem*)));
//    connect(ui->TextAnlass, SIGNAL(textChanged(QString)), this, SLOT(unSichern()));

    Farben << QColor(255, 255, 255) << QColor(205,92,92) << QColor(238,130,238) << QColor(211, 211, 211) << QColor(154, 205, 50) << QColor(255, 165, 0) << QColor(135, 206, 250);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_ButtonAdd_clicked ()
{
    QFahrtag *neu_Fahrtag = new QFahrtag();
    QListWidgetItem *neu_Feld = new QListWidgetItem();
    neu_Feld->setText(neu_Fahrtag->getDatum().toString("dddd d. M. yyyy"));
    ui->ListeZuege->addItem(neu_Feld);
    Fahrtage.insert(neu_Feld, neu_Fahrtag);

    aktualisieren();
}

void MainWindow::bearbeiten(QListWidgetItem* item_x) {
    setState(true);

    QFahrtag *item = Fahrtage[item_x];

    if (aktuellerZug != item_x) {
        if (genommen == false){
            if (QMessageBox::question(NULL, tr("Änderungen übernehmen?"), tr("Möchten Sie die Änderungen übernehmen?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
                ZugSichern();
            }
        }
        if (genommen) {
            ZugLaden(item_x);
            genommen = false;
        }
    }

}
void MainWindow::ZugLaden(QListWidgetItem* zug) {
    aktuellerZug = zug;
    QFahrtag *akt = Fahrtage[aktuellerZug];

    ui->DateZug->setDate(akt->getDatum());
    ui->TextAnlass->setText(akt->getAnlass());
    ui->ComboArt->setCurrentIndex(akt->getArt());
    ui->CheckWichtig->setChecked(akt->getWichtig());
// Wagenreihung
    /*
    ui->TextTf->clear();
    ui->TextTf->insertPlainText(akt->getTextTf());
    ui->TextZf->clear();
    ui->TextZf->insertPlainText(akt->getTextZf());
    ui->TextZub->clear();
    ui->TextZub->insertPlainText(akt->getTextZub());
    ui->TextService->clear();
    ui->TextService->insertPlainText(akt->getTextService());
*/
    aktualisieren();
}

void MainWindow::on_ButtonNehmen_clicked()
{
    ZugSichern();
}

void MainWindow::ZugSichern()
{
    QFahrtag *akt = Fahrtage[aktuellerZug];

    akt->setDatum(ui->DateZug->date());
    akt->setAnlass(ui->TextAnlass->text());
    akt->setArt(ui->ComboArt->currentIndex());
    akt->setWichtig(ui->CheckWichtig->isChecked());
// Wagenreihung
   /* akt->setTextTf(ui->TextTf->toPlainText());
    akt->setTextZf(ui->TextZf->toPlainText());
    akt->setTextZub(ui->TextZub->toPlainText());
    akt->setTextService(ui->TextService->toPlainText());
   */ //
    //
    //
    //

    aktualisieren();

    genommen = true;

}

void MainWindow::aktualisieren()
{
    foreach(QListWidgetItem *zug, Fahrtage.keys()) {

        QFahrtag *akt = Fahrtage[zug];
        zug->setText(akt->getDatum().toString("yyyy M. d. dddd"));
        zug->setBackgroundColor(Farben.at(akt->getArt()));

    }
    ui->ListeZuege->sortItems();
}

void MainWindow::setState(bool aktiv)
{
    ui->DateZug->setEnabled(aktiv);
    ui->ComboArt->setEnabled(aktiv);
    ui->ComboWagenreihung->setEnabled(aktiv);
    ui->CheckWichtig->setEnabled(aktiv);
    ui->TextAnlass->setEnabled(aktiv);
    ui->TextBemerkungen->setEnabled(aktiv);
    ui->TextReservierungen->setEnabled(aktiv);
  /*  ui->TextService->setEnabled(aktiv);
    ui->TextTf->setEnabled(aktiv);
    ui->TextZf->setEnabled(aktiv);
    ui->TextZub->setEnabled(aktiv);
   */ ui->TimeTf->setEnabled(aktiv);
    ui->TimeZ->setEnabled(aktiv);

    ui->ButtonNehmen->setEnabled(aktiv);
}


