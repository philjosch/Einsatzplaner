#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include "export.h"
#include "minimumhourseditordialog.h"

#include <QHash>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPrinter>
#include <QSettings>
#include <QDebug>

const QString PersonalWindow::nichtGenugStunden = "#ff9999";

PersonalWindow::PersonalWindow(QWidget *parent, ManagerPersonal *m) : QMainWindow(parent), ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);
    connect(ui->pushMindeststunden, &QAbstractButton::clicked, this, &PersonalWindow::editMinimumHours);

    // Initalisieren der Statischen variablen
    manager = m;
    setWindowTitle(tr("Personalmanagement"));

    itemToPerson = new QHash<QListWidgetItem*, Person*>();
    personToItem = new QHash<Person*, QListWidgetItem*>();
    enabled = false;

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

    anzeige = new QList<bool>();
    while(anzeige->length() <= 11) {
        anzeige->append(false);
    }
    ui->checkShowGesamt->setChecked(true);
    ui->checkShowAnzahl->setChecked(true);
    ui->checkShowKilometer->setChecked(true);
    on_checkShowGesamt_clicked(true);
    on_checkShowAnzahl_clicked(true);
    on_checkShowKilometer_clicked(true);
    ui->tabelleGesamt->sortItems(1);
}

PersonalWindow::~PersonalWindow()
{
    delete ui;
}

void PersonalWindow::showPerson(Person *p)
{
    aktuellePerson = p;

    p->berechne();

    enabled = false;
    ui->lineVorname->setEnabled(true);
    ui->lineNachname->setEnabled(true);
    ui->spinKm->setEnabled(true);
    ui->checkRangierer->setEnabled(true);
    ui->checkTf->setEnabled(true);
    ui->checkZf->setEnabled(true);
    ui->tabelle->setEnabled(true);
    ui->lineVorname->setText(p->getVorname());
    ui->lineNachname->setText(p->getNachname());
    ui->spinKm->setValue(p->getStrecke());
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());
    // Zusätzliche Zeiten aktivieren
    ui->doubleTf->setEnabled(true);
    ui->doubleZf->setEnabled(true);
    ui->doubleService->setEnabled(true);
    ui->doubleZub->setEnabled(true);
    ui->doubleZugVorbereiten->setEnabled(true);
    ui->doubleWerkstatt->setEnabled(true);
    ui->doubleBuero->setEnabled(true);
    ui->doubleAusbildung->setEnabled(true);
    ui->doubleSonstiges->setEnabled(true);
    ui->doubleKilometer->setEnabled(true);

    // Tabellendaten laden und einfügen
    while(ui->tabelle->rowCount() > 0) ui->tabelle->removeRow(0);
    QListIterator<AActivity*> *i = p->getActivities();
    bool sortingSaved = ui->tabelle->isSortingEnabled();
    ui->tabelle->setSortingEnabled(false);
    while(i->hasNext()) {
        AActivity *a = i->next();
        // Datum
        ui->tabelle->insertRow(0);
        QTableWidgetItem *i0 = new QTableWidgetItem();
        i0->setData(Qt::EditRole, a->getDatum());
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
    ui->tabelle->setSortingEnabled(sortingSaved);
    ui->tabelle->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->lineTf->setText(QString::number(p->getTimeTf()));
    ui->lineZf->setText(QString::number(p->getTimeZf()));
    ui->lineZub->setText(QString::number(p->getTimeZub()));
    ui->lineService->setText(QString::number(p->getTimeService()));
    ui->lineZugVorbereiten->setText(QString::number(p->getTimeVorbereiten()));
    ui->lineWerkstatt->setText(QString::number(p->getTimeWerkstatt()));
    ui->lineBuero->setText(QString::number(p->getTimeBuero()));
    ui->lineAusbildung->setText(QString::number(p->getTimeAusbildung()));
    ui->lineSonstiges->setText(QString::number(p->getTimeSonstiges()));
    ui->lineKilometer->setText(QString::number(p->getSumKilometer()));
    ui->lineGesamt->setText(QString::number(p->getTimeSum()));

    ui->doubleTf->setValue(p->getAdditionalTimeTf());
    ui->doubleZf->setValue(p->getAdditionalTimeZf());
    ui->doubleZub->setValue(p->getAdditionalTimeZub());
    ui->doubleService->setValue(p->getAdditionalTimeService());
    ui->doubleZugVorbereiten->setValue(p->getAdditionalTimeVorbereiten());
    ui->doubleWerkstatt->setValue(p->getAdditionalTimeWerkstatt());
    ui->doubleBuero->setValue(p->getAdditionalTimeBuero());
    ui->doubleAusbildung->setValue(p->getAdditionalTimeAusbildung());
    ui->doubleSonstiges->setValue(p->getAdditionalTimeSonstiges());
    ui->doubleKilometer->setValue(p->getAdditionalKilometer());

    enabled = true;
}

void PersonalWindow::refresh()
{
    // Aktualisiere die Ansichten
    refreshGesamt();
    refreshEinzel();
}

void PersonalWindow::loadData()
{
    QSetIterator<Person*> i = manager->getPersonen();
    while(i.hasNext()) {
        Person *p = i.next();
        QListWidgetItem *item = new QListWidgetItem(p->getName());
        ui->listWidget->insertItem(ui->listWidget->count(), item);
        itemToPerson->insert(item, p);
        personToItem->insert(p, item);
    }
    refresh();
}

void PersonalWindow::on_pushAktualisieren_clicked()
{
    refreshGesamt();
}

void PersonalWindow::refreshGesamt()
{
    while(ui->tabelleGesamt->rowCount() > 0) {
        ui->tabelleGesamt->removeRow(0);
    }
    QSetIterator<Person*> iterator = manager->getPersonen();
    ui->tabelleGesamt->setSortingEnabled(false);
    //  0: summe gesamt
    //  1: anzahl
    //  2: tf/tb
    //  3: zf
    //  4: zub/begl.o.b.a.
    //  5: service
    //  6: zug vorbereiten
    //  7: werkstatt
    //  8: büro
    //  9: sonstiges
    // 10: kilometer
    // 11: ausbildung
    while (ui->tabelleGesamt->columnCount() > 2) {
        ui->tabelleGesamt->removeColumn(2);
    }

    if (anzeige->at(10)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Kilometer")));
    }
    if (anzeige->at(9)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Sonstiges")));
    }
    if (anzeige->at(11)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Ausbildung")));
    }
    if (anzeige->at(8)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Büro")));
    }
    if (anzeige->at(7)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Werkstatt")));
    }
    if (anzeige->at(6)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Zug\nVorbereiten")));
    }
    if (anzeige->at(5)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Service")));
    }
    if (anzeige->at(4)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Zub")));
    }
    if (anzeige->at(3)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Zf")));
    }
    if (anzeige->at(2)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Tf/Tb")));
    }
    if (anzeige->at(1)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Anzahl")));
    }
    if (anzeige->at(0)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Gesamt")));
    }
    ui->tabelleGesamt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Insert the sum of each column
    manager->berechne();

    ui->tabelleGesamt->insertRow(0);
    QTableWidgetItem *ii = new QTableWidgetItem();
    ui->tabelleGesamt->setItem(0, 0, ii);

    ii = new QTableWidgetItem("Gesamt");
    ui->tabelleGesamt->setItem(0, 1, ii);

    int pos = 2;
    if (anzeige->at(0)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeSum());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(1)) {
        ii = new QTableWidgetItem();
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(2)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeTf());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(3)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeZf());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(4)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeZub());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(5)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeService());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(6)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeVorbereiten());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(7)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeWerkstatt());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(8)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeBuero());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(11)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeAusbildung());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(9)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getTimeSonstiges());
        ui->tabelleGesamt->setItem(0, pos, ii);
        pos++;
    }
    if (anzeige->at(10)) {
        ii = new QTableWidgetItem();
        ii->setData(Qt::EditRole, manager->getSumKilometer());
        ui->tabelleGesamt->setItem(0, pos, ii);
    }
    // End of insert the sum of each column


    while(iterator.hasNext()) {
        Person *p = iterator.next();
        QString farbe = "#ffffff";
        if (! manager->pruefeStunden(p)) {
            farbe = nichtGenugStunden;
        }
        ui->tabelleGesamt->insertRow(0);
        QTableWidgetItem *i = new QTableWidgetItem(p->getVorname());
        i->setBackgroundColor(farbe);
        ui->tabelleGesamt->setItem(0, 0, i);

        i = new QTableWidgetItem(p->getNachname());
        i->setBackgroundColor(farbe);
        ui->tabelleGesamt->setItem(0, 1, i);

        int pos = 2;
        if (anzeige->at(0)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeSum());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(1)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getAnzahl());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(2)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeTf());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(3)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeZf());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(4)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeZub());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(5)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeService());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(6)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeVorbereiten());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(7)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeWerkstatt());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(8)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeBuero());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(9)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getTimeSonstiges());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
            pos++;
        }
        if (anzeige->at(10)) {
            i = new QTableWidgetItem();
            i->setData(Qt::EditRole, p->getSumKilometer());
            i->setBackgroundColor(farbe);
            ui->tabelleGesamt->setItem(0, pos, i);
        }
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
    Person *p = manager->registerPerson("Unbenannt", "Unbekannt");
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
    emit changed();
}

void PersonalWindow::on_lineVorname_textChanged(const QString &arg1)
{
    if (enabled) {
        // test, ob Person vorhanden ist
        if (manager->personExists(arg1, aktuellePerson->getNachname())) {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        } else {
            aktuellePerson->setVorname(arg1);
            personToItem->value(aktuellePerson)->setText(aktuellePerson->getName());
            ui->listWidget->sortItems();
        }
        emit changed();
    }
}

void PersonalWindow::on_lineNachname_textChanged(const QString &arg1)
{
    if (enabled) {
        // test, ob Person vorhanden ist
        if (manager->personExists(aktuellePerson->getVorname(), arg1)) {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        } else {
            aktuellePerson->setNachname(arg1);
            personToItem->value(aktuellePerson)->setText(aktuellePerson->getName());
            ui->listWidget->sortItems();
        }
        emit changed();
    }
}

void PersonalWindow::on_spinKm_valueChanged(int arg1)
{
    if (enabled) {
        aktuellePerson->setStrecke(arg1);
        ui->lineKilometer->setText(QString::number(aktuellePerson->getSumKilometer()));
        emit changed();
    }
}

void PersonalWindow::on_checkTf_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungTf(checked);
        emit changed();
    }
}

void PersonalWindow::on_checkZf_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungZf(checked);
        emit changed();
    }
}

void PersonalWindow::on_checkRangierer_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungRangierer(checked);
        emit changed();
    }
}

void PersonalWindow::on_pushDelete_clicked()
{
    // Lösche die Person
    if (ui->listWidget->selectedItems().length() > 0 && enabled) {
        enabled = false;
        QListWidgetItem *i = ui->listWidget->selectedItems().at(0);
        Person *p = itemToPerson->value(i);
        if (p->getAnzahl() > 0) {
            QMessageBox::information(this, tr("Warnung"), tr("Die ausgewählte Person kann nicht gelöscht werden, da Sie noch bei Aktivitäten eingetragen ist.\nBitte lösen Sie diese Verbindung bevor Sie die Person löschen!"));
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
            disableFields();
            ui->pushDelete->setEnabled(false);
        } else {
            aktuellePerson = itemToPerson->value(ui->listWidget->item(0));
            showPerson(aktuellePerson);
            enabled = true;
        }
        emit changed();
    }
}

void PersonalWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    showPerson(itemToPerson->value(item));
}

void PersonalWindow::on_pushPDF_clicked()
{
    QPrinter *pdf = 0;
    pdf = Export::getPrinterPDF(this, "Personal-Gesamt.pdf");
    print(pdf);
}

void PersonalWindow::on_pushPrint_clicked()
{
    QPrinter *paper = 0;
    paper = Export::getPrinterPaper(this);
    print(paper);
}

void PersonalWindow::on_pushPDFEinzel_clicked()
{
    QPrinter *pdf = 0;
    pdf = Export::getPrinterPDF(this, "Personal-Einzelansicht.pdf");
    Export::printPerson(manager, pdf);
}

void PersonalWindow::on_pushPrintEinzel_clicked()
{
    QPrinter *paper = 0;
    paper = Export::getPrinterPaper(this);
    Export::printPerson(manager, paper);
}

void PersonalWindow::print(QPrinter *p)
{
    refreshGesamt();
    QList<Person*> *liste = new QList<Person*>();

    for(int i = 0; i < ui->tabelleGesamt->rowCount(); i++) {
        QString name = ui->tabelleGesamt->item(i, 0)->text();
        if (name != "") name += " ";
        name += ui->tabelleGesamt->item(i, 1)->text();
        Person *pers = manager->getPerson(name);
        if (pers != nullptr)
            liste->append(pers);
    }
    QList<double> *gesamt = new QList<double>();
    gesamt->append(manager->getTimeSum());
    gesamt->append(0);
    gesamt->append(manager->getTimeTf());
    gesamt->append(manager->getTimeZf());
    gesamt->append(manager->getTimeZub());
    gesamt->append(manager->getTimeService());
    gesamt->append(manager->getTimeVorbereiten());
    gesamt->append(manager->getTimeWerkstatt());
    gesamt->append(manager->getTimeBuero());
    gesamt->append(manager->getTimeSonstiges());
    gesamt->append(manager->getSumKilometer());
    Export::printPersonen(liste, gesamt, anzeige, p);
}

void PersonalWindow::disableFields()
{
    ui->lineVorname->setEnabled(false);
    ui->lineNachname->setEnabled(false);
    ui->spinKm->setEnabled(false);
    ui->checkRangierer->setEnabled(false);
    ui->checkTf->setEnabled(false);
    ui->checkZf->setEnabled(false);
    ui->tabelle->setEnabled(false);

    ui->doubleTf->setEnabled(false);
    ui->doubleZf->setEnabled(false);
    ui->doubleZub->setEnabled(false);
    ui->doubleService->setEnabled(false);
    ui->doubleZugVorbereiten->setEnabled(false);
    ui->doubleWerkstatt->setEnabled(false);
    ui->doubleBuero->setEnabled(false);
    ui->doubleAusbildung->setEnabled(false);
    ui->doubleSonstiges->setEnabled(false);
    ui->doubleKilometer->setEnabled(false);

    ui->lineTf->setEnabled(false);
    ui->lineZf->setEnabled(false);
    ui->lineZub->setEnabled(false);
    ui->lineService->setEnabled(false);
    ui->lineZugVorbereiten->setEnabled(false);
    ui->lineWerkstatt->setEnabled(false);
    ui->lineBuero->setEnabled(false);
    ui->lineAusbildung->setEnabled(false);
    ui->lineSonstiges->setEnabled(false);
    ui->lineKilometer->setEnabled(false);

    ui->lineGesamt->setEnabled(false);
}

void PersonalWindow::on_tabWidgetMain_tabBarClicked(int index)
{
    if (index == 1)
        refreshEinzel();
    if (index == 0)
        refreshGesamt();
}

void PersonalWindow::on_checkShowGesamt_clicked(bool checked)
{
    anzeige->replace(0, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowAnzahl_clicked(bool checked)
{
    anzeige->replace(1, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowTf_clicked(bool checked)
{
    anzeige->replace(2, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowZf_clicked(bool checked)
{
    anzeige->replace(3, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowZub_clicked(bool checked)
{
    anzeige->replace(4, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowService_clicked(bool checked)
{
    anzeige->replace(5, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowVorbereiten_clicked(bool checked)
{
    anzeige->replace(6, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowWerkstatt_clicked(bool checked)
{
    anzeige->replace(7, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowBuero_clicked(bool checked)
{
    anzeige->replace(8, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowSonstiges_clicked(bool checked)
{
    anzeige->replace(9, checked);
    refreshGesamt();
}

void PersonalWindow::on_checkShowKilometer_clicked(bool checked)
{
    anzeige->replace(10, checked);
    refreshGesamt();
}

void PersonalWindow::on_doubleTf_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeTf(arg1);
        ui->lineTf->setText(QString::number(aktuellePerson->getTimeTf()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleZf_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeZf(arg1);
        ui->lineZf->setText(QString::number(aktuellePerson->getTimeZf()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleZub_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeZub(arg1);
        ui->lineZub->setText(QString::number(aktuellePerson->getTimeZub()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleService_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeService(arg1);
        ui->lineService->setText(QString::number(aktuellePerson->getTimeService()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleZugVorbereiten_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeVorbereiten(arg1);
        ui->lineZugVorbereiten->setText(QString::number(aktuellePerson->getTimeVorbereiten()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleWerkstatt_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeWerkstatt(arg1);
        ui->lineWerkstatt->setText(QString::number(aktuellePerson->getTimeWerkstatt()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleBuero_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeBuero(arg1);
        ui->lineBuero->setText(QString::number(aktuellePerson->getTimeBuero()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleAusbildung_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeAusbildung(arg1);
        ui->lineAusbildung->setText(QString::number(aktuellePerson->getTimeAusbildung()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleSonstiges_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalTimeSonstiges(arg1);
        ui->lineSonstiges->setText(QString::number(aktuellePerson->getTimeSonstiges()));
        ui->lineGesamt->setText(QString::number(aktuellePerson->getTimeSum()));
        emit changed();
    }
}

void PersonalWindow::on_doubleKilometer_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalKilometer(arg1);
        ui->lineKilometer->setText(QString::number(aktuellePerson->getSumKilometer()));
        emit changed();
    }
}

void PersonalWindow::editMinimumHours()
{
   if (MinimumHoursEditorDialog(manager, this).exec() == QDialog::Accepted) {
       on_pushAktualisieren_clicked();
       changed();
   }
}
