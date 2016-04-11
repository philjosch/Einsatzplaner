#include "reservierungenuebersicht.h"
#include "ui_reservierungenuebersicht.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDir>
#include <QFileDialog>

ReservierungenUebersicht::ReservierungenUebersicht(ManagerReservierungen *manager, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReservierungenUebersicht)
{
    ui->setupUi(this);
    this->manager = manager;
    reservierungChanged();
}

ReservierungenUebersicht::~ReservierungenUebersicht()
{
    delete ui;
}

void ReservierungenUebersicht::reservierungChanged()
{
    if (manager == NULL)
        return;
    setTable();
    setGrafik();
}

void ReservierungenUebersicht::setManager(ManagerReservierungen *value)
{
    manager = value;
    reservierungChanged();
}

void ReservierungenUebersicht::cleanTable()
{
    ui->tableWidget->setRowCount(0);
}
void ReservierungenUebersicht::cleanGrafik()
{
}


void ReservierungenUebersicht::setTable()
{
    cleanTable();
    if (ui->radioListName->isChecked()) manager->sortName();
    else manager->sortPlatz();

    ui->tableWidget->setSortingEnabled(false);
    QList<Reservierung*> *reser = manager->getReservierungen();
    for(int i = 0; i < reser->length(); i++) {
        Reservierung *res = reser->at(i);
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(res->getName()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(res->getAnzahl())+" Plätze "+res->getKlasse()+"\n"+res->getSitzplatz()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(res->getStartZug().at(0)+" "+res->getStartHp().at(0)+"\n->"+res->getEndeZug().at(0)+" "+res->getEndeHp().at(0)));

        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(res->getTelefon()+(res->getMail().length()>0 ? " | "+res->getMail() : " " )));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(res->getSonstiges()+(res->getFahrrad() ? " Nehmen Fahrräder mit!" : " ")));
    }
}

void ReservierungenUebersicht::setGrafik()
{

}

void ReservierungenUebersicht::on_pushDrucken_clicked()
{

}

void ReservierungenUebersicht::on_pushPDF_clicked()
{
    // TextDocument erstellen und dann eine HTML-Tabelle einfügen
    // Diese dann Drucken
    QPrinter printer;
    QPrintDialog printer_dialog(&printer);
    if (printer_dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        ui->tableWidget->render(&painter);
    }

}

void ReservierungenUebersicht::on_radioListName_clicked()
{
    ui->radioListName->setChecked(true);
    ui->radioListPlatz->setChecked(false);
    setTable();
}

void ReservierungenUebersicht::on_radioListPlatz_clicked()
{
    ui->radioListName->setChecked(false);
    ui->radioListPlatz->setChecked(true);
    setTable();
}
