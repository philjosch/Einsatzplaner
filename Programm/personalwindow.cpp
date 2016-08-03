#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include <QHash>
#include <QListWidgetItem>


PersonalWindow::PersonalWindow(QWidget *parent, ManagerPersonal *m) :
    QMainWindow(parent),
    ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);
    manager = m;
//    setWindowFilePath(parent->windowFilePath());
    setWindowTitle("Personalmanagement");

    itemToPerson = new QHash<QListWidgetItem*, Person*>();
    personToItem = new QHash<Person*, QListWidgetItem*>();

}

PersonalWindow::~PersonalWindow()
{
    delete ui;
}

void PersonalWindow::showPerson(Person *p)
{
    ui->lineName->setText(p->getName());
    ui->spinKm->setValue(p->getStrecke());
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());

    // Tabellendaten laden und einfügen
    ui->lineTf->setText(QString::number(p->getTimeBuero()));
    ui->lineZf->setText(QString::number(p->getTimeBuero()));
    ui->lineService->setText(QString::number(p->getTimeBuero()));
    ui->lineWerkstatt->setText(QString::number(p->getTimeBuero()));
    ui->lineBuero->setText(QString::number(p->getTimeBuero()));
    ui->lineGesamt->setText(QString::number(p->getTimeSum()));

    ui->lineBuero->setText(QString::number(p->getTimeBuero()));

}

void PersonalWindow::on_pushAktualisieren_clicked()
{

}

void PersonalWindow::refreshGesamt()
{
    ui->tabelleGesamt->clear();

}

void PersonalWindow::refreshEinzel()
{

}

void PersonalWindow::on_pushAdd_clicked()
{
    Person *p = manager->registerPerson("Unbenannt Unbekannt");
    if (p == nullptr) {
        return;
    }
    aktuellePerson = p;
    QListWidgetItem *item = new QListWidgetItem(aktuellePerson->getName());
    ui->listWidget->insertItem(0, item);
    itemToPerson->insert(item, aktuellePerson);
    personToItem->insert(aktuellePerson, item);
    showPerson(aktuellePerson);
}
