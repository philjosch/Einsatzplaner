#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include <QHash>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPrinter>
#include "export.h"
#include <QSettings>
#include <QDebug>

const QString PersonalWindow::nichtGenugStunden = "#ff9999";

PersonalWindow::PersonalWindow(QWidget *parent, ManagerPersonal *m) :
    QMainWindow(parent),
    ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);

    // Initalisieren der Statischen variablen
    manager = m;
    setWindowTitle("Personalmanagement");

    itemToPerson = new QHash<QListWidgetItem*, Person*>();
    personToItem = new QHash<Person*, QListWidgetItem*>();
    enabled = false;

    refreshGesamt();
    QSetIterator<Person*> i = manager->getPersonen();
    while(i.hasNext()) {
        Person *p = i.next();
        QListWidgetItem *item = new QListWidgetItem(p->getName());
        ui->listWidget->insertItem(0, item);
        itemToPerson->insert(item, p);
        personToItem->insert(p, item);
        ui->pushDelete->setEnabled(true);
    }
    refreshEinzel();

    QSettings settings;
    if (settings.value("window/personal/x").toInt() != 0) {
        this->setGeometry(settings.value("window/personal/x").toInt(),
                          settings.value("window/personal/y").toInt(),
                          settings.value("window/personal/width").toInt(),
                          settings.value("window/personal/height").toInt());
    }
    if (! settings.value("window/personal/visible").toBool()) {
        this->hide();
    } else {
        this->show();
    }
}

PersonalWindow::~PersonalWindow()
{
    QSettings settings;
    settings.setValue("window/personal/x", this->x());
    settings.setValue("window/personal/y", this->y());
    settings.setValue("window/personal/width", this->width());
    settings.setValue("window/personal/height", this->height());
    settings.setValue("window/personal/visible", this->isVisible());
    delete ui;
}

void PersonalWindow::showPerson(Person *p)
{
    enabled = false;
    ui->lineName->setEnabled(true);
    ui->spinKm->setEnabled(true);
    ui->checkRangierer->setEnabled(true);
    ui->checkTf->setEnabled(true);
    ui->checkZf->setEnabled(true);
    ui->tabelle->setEnabled(true);
    aktuellePerson = p;
    ui->lineName->setText(p->getName());
    ui->spinKm->setValue(p->getStrecke());
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());

    // Tabellendaten laden und einfügen
    while(ui->tabelle->rowCount() > 0) ui->tabelle->removeRow(0);
    QListIterator<AActivity*> *i = p->getActivities();
    while(i->hasNext()) {
        AActivity *a = i->next();
        // Datum
        ui->tabelle->insertRow(0);
        QTableWidgetItem *i0 = new QTableWidgetItem(a->getDatum()->toString("dd.MM.yyyy"));
        ui->tabelle->setItem(0, 0, i0);

        AActivity::Infos *infos = a->getIndividual(p);

        // Aufgabe
        QTableWidgetItem *i1 = new QTableWidgetItem(AActivity::getStringFromCategory(infos->kategorie));
        ui->tabelle->setItem(0, 1, i1);

        // Einsatzstunden
        QTime start = infos->beginn;
        QTime ende = infos->ende;

        QTime duration = QTime::fromMSecsSinceStartOfDay(start.msecsTo(ende));

        QTableWidgetItem *i2 = new QTableWidgetItem(duration.toString("hh:mm"));
        ui->tabelle->setItem(0, 2, i2);

        // Beschreibung
        QTableWidgetItem *i3 = new QTableWidgetItem(a->getAnlass());
        ui->tabelle->setItem(0, 3, i3);
    }

    ui->lineTf->setText(QString::number(p->getTimeTf()));
    ui->lineZf->setText(QString::number(p->getTimeZf()));
    ui->lineService->setText(QString::number(p->getTimeService()));
    ui->lineWerkstatt->setText(QString::number(p->getTimeWerkstatt()));
    ui->lineBuero->setText(QString::number(p->getTimeBuero()));
    ui->lineGesamt->setText(QString::number(p->getTimeSum()));

    ui->lineStrecke->setText(QString::number(p->getSumKilometer()));

    enabled = true;
}

void PersonalWindow::on_pushAktualisieren_clicked()
{
    // Aktualisiere die Ansichten
    refreshGesamt();
    refreshEinzel();
}

void PersonalWindow::refreshGesamt()
{
//    ui->tabelleGesamt->clear();
    while(ui->tabelleGesamt->rowCount() > 0) {
        ui->tabelleGesamt->removeRow(0);
    }
    QSetIterator<Person*> i = manager->getPersonen();
    ui->tabelleGesamt->setSortingEnabled(false);
    while(i.hasNext()) {
        Person *p = i.next();
        p->berechne();
        QString farbe = "#ffffff";
        if (! manager->pruefeStunden(p)) {
            farbe = nichtGenugStunden;
        }
        ui->tabelleGesamt->insertRow(0);
        QTableWidgetItem *i = new QTableWidgetItem(p->getName());
        i->setBackgroundColor(farbe);
        ui->tabelleGesamt->setItem(0, 0, i);

        i = new QTableWidgetItem(QString::number(p->getTimeSum()));
        i->setBackgroundColor(farbe);
        ui->tabelleGesamt->setItem(0, 1, i);

        i = new QTableWidgetItem(QString::number(p->getSumKilometer()));
        i->setBackgroundColor(farbe);
        ui->tabelleGesamt->setItem(0, 2, i);

        i = new QTableWidgetItem(QString::number(p->getAnzahl()));
        i->setBackgroundColor(farbe);
        ui->tabelleGesamt->setItem(0, 3, i);
    }
    ui->tabelleGesamt->setSortingEnabled(true);
}

void PersonalWindow::refreshEinzel()
{
    // hier müssen nur die Farben angepasst werden
    for(int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *item = ui->listWidget->item(i);
        Person *p = itemToPerson->value(item);
        if (manager->pruefeStunden(p)) {
            item->setBackgroundColor(QColor("#ffffff"));
        } else {
            item->setBackgroundColor(QColor(nichtGenugStunden));
        }
    }
    ui->listWidget->sortItems();
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
    ui->pushDelete->setEnabled(true);
    refreshEinzel();
}

void PersonalWindow::on_lineName_textChanged(const QString &arg1)
{
    if (enabled) {
        if (manager->personExists(arg1)) {
            QMessageBox::information(this, "Name doppelt vergeben", "Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!");
        } else {
            aktuellePerson->setName(arg1);
            personToItem->value(aktuellePerson)->setText(arg1);
            ui->listWidget->sortItems();
        }
    }
}

void PersonalWindow::on_spinKm_valueChanged(int arg1)
{
    if (enabled)
    aktuellePerson->setStrecke(arg1);
}

void PersonalWindow::on_checkTf_clicked(bool checked)
{
    if (enabled)
        aktuellePerson->setAusbildungTf(checked);
}

void PersonalWindow::on_checkZf_clicked(bool checked)
{
    if (enabled)
        aktuellePerson->setAusbildungZf(checked);
}

void PersonalWindow::on_checkRangierer_clicked(bool checked)
{
    if (enabled)
        aktuellePerson->setAusbildungRangierer(checked);
}

void PersonalWindow::on_pushDelete_clicked()
{
    // Lösche die Person
    if (ui->listWidget->selectedItems().length() > 0) {
        enabled = false;
        QListWidgetItem *i = ui->listWidget->selectedItems().at(0);
        Person *p = itemToPerson->value(i);
        if (p->getAnzahl() > 0) {
            QMessageBox::information(this, "Warnung", "Die ausgewählte Person kann nciht gelöscht werden, da Sie noch bei Aktivitäten eingetragen ist.\nBitte lösen Sie diese Verbindung bevor Sie die Person löschen!");
            enabled = true;
            return;
        }
        if (p == aktuellePerson) {
            aktuellePerson = nullptr;
        }
        itemToPerson->remove(i);
        personToItem->remove(p);
        manager->removePerson(p);
        delete p;
        ui->listWidget->takeItem(ui->listWidget->row(i));

        if (ui->listWidget->count() == 0) {
            ui->lineName->setEnabled(false);
            ui->spinKm->setEnabled(false);
            ui->checkRangierer->setEnabled(false);
            ui->checkTf->setEnabled(false);
            ui->checkZf->setEnabled(false);
            ui->tabelle->setEnabled(false);

            ui->pushDelete->setEnabled(false);
        } else {
            aktuellePerson = itemToPerson->value(ui->listWidget->item(0));
            showPerson(aktuellePerson);
            enabled = true;
        }
    }
}

void PersonalWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    showPerson(itemToPerson->value(item));
}

void PersonalWindow::on_pushPDF_clicked()
{
    QPrinter *pdf = 0;
    pdf = Export::getPrinterPDF(this, "Personal.pdf");
    print(pdf);
}

void PersonalWindow::on_pushPrint_clicked()
{
    QPrinter *paper = 0;
    paper = Export::getPrinterPaper(this);
    print(paper);
}

void PersonalWindow::print(QPrinter *p)
{
    QList<Person*> *liste = new QList<Person*>();
    for(int i = 0; i < ui->tabelleGesamt->rowCount(); i++) {
        liste->append(manager->getPerson(ui->tabelleGesamt->item(i, 0)->text()));
    }
    Export::printPersonen(liste, p);
}
