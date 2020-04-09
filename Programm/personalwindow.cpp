#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include "export.h"
#include "minimumhourseditordialog.h"

#include <QMessageBox>
#include <math.h>
#include <cmath>
#include <QDesktopServices>

const QString PersonalWindow::nichtGenugStunden = "#ff9999";
const QString PersonalWindow::genugStunden = "#99ff99";
const QList<Category> PersonalWindow::anzeigeReihenfolge = {Category::Gesamt, Category::Anzahl, Category::Tf, Category::Zf, Category::Zub, Category::Service, Category::ZugVorbereiten, Category::Werkstatt, Category::Buero, Category::Ausbildung, Category::Infrastruktur, Category::Sonstiges, Category::Kilometer};

PersonalWindow::PersonalWindow(QWidget *parent, ManagerPersonal *m) : QMainWindow(parent), ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);
    connect(ui->pushMindeststunden, &QAbstractButton::clicked, this, &PersonalWindow::editMinimumHours);

    // Initalisieren der Statischen variablen
    manager = m;
    setWindowTitle(tr("Personalmanagement"));

    itemToPerson = QHash<QListWidgetItem*, Person*>();
    personToItem = QHash<Person*, QListWidgetItem*>();
    enabled = false;

    current = QList<Person*>();
    QListIterator<Person*> i = manager->getPersonen();
    while(i.hasNext()) {
        current.append(i.next());
    }
    refreshEinzel();

    anzeige = new QSet<Category>();
    ui->checkShowGesamt->setChecked(true);
    ui->checkShowAnzahl->setChecked(true);
    ui->checkShowKilometer->setChecked(true);
    on_checkShowGesamt_clicked(true);
    on_checkShowAnzahl_clicked(true);
    on_checkShowKilometer_clicked(true);
    ui->comboEinzel->setCurrentIndex(1);
    on_comboEinzel_currentIndexChanged(1);
    ui->tabelleGesamt->sortItems(1);
    aktuellePerson = nullptr;
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
    toggleFields(true);

    // ** Kopfzeile
    ui->lineID->setText(QString::number(p->getNummer()));
    ui->lineVorname->setText(p->getVorname());
    ui->lineNachname->setText(p->getNachname());

    // ** Stammdaten
    // Allgemein
    ui->dateBirth->setDate(p->getGeburtstag());
    ui->dateEntry->setDate(p->getEintritt());
    ui->checkAktiv->setChecked(p->getAktiv());
    ui->spinKm->setValue(p->getStrecke());
    ui->lineJob->setText(p->getBeruf());

    // Kontakt
    ui->linePhone->setText(p->getTelefon());
    ui->checkPhone->setChecked(p->getTelefonOK());
    ui->lineMail->setText(p->getMail());
    ui->checkMail->setChecked(p->getMailOK());

    // Betriebsdienst
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());
    ui->dateDienst->setDate(p->getTauglichkeit());

    // Sonstiges
    ui->plainBemerkung->setPlainText(p->getBemerkungen());
    ui->dateExit->setDate(p->getAustritt());

    // ** Aktivitaeten
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

        Infos infos = a->getIndividual(p);

        // Aufgabe
        QTableWidgetItem *i1 = new QTableWidgetItem(getLocalizedStringFromCategory(infos.kategorie));
        ui->tabelle->setItem(0, 1, i1);

        // Einsatzstunden
        QTime start = infos.beginn;
        QTime ende = infos.ende;

        QTime duration = QTime::fromMSecsSinceStartOfDay(start.msecsTo(ende));

        QTableWidgetItem *i2 = new QTableWidgetItem(duration.toString("hh:mm"));
        ui->tabelle->setItem(0, 2, i2);

        // Beschreibung
        QTableWidgetItem *i3 = new QTableWidgetItem(a->getAnlass());
        ui->tabelle->setItem(0, 3, i3);
    }
    ui->tabelle->setSortingEnabled(sortingSaved);
    ui->tabelle->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // ** Konto
    ui->labelTfSum->setText(p->getString(Tf));
    ui->labelZfSum->setText(p->getString(Zf));
    ui->labelZubSum->setText((p->getString(Zub)));
    ui->labelServiceSum->setText((p->getString(Service)));
    ui->labelZugVorbereitenSum->setText((p->getString(ZugVorbereiten)));
    ui->labelWerkstattSum->setText((p->getString(Werkstatt)));
    ui->labelBueroSum->setText(p->getString(Buero));
    ui->labelAusbildungSum->setText(p->getString(Ausbildung));
    ui->labelInfrastrukturSum->setText(p->getString(Infrastruktur));
    ui->labelSonstigesSum->setText(p->getString(Sonstiges));
    ui->labelAnzahlSum->setText(p->getString(Anzahl));
    ui->labelKilometerSum->setText(p->getString(Kilometer));
    ui->labelGesamt->setText(p->getString(Gesamt));

    ui->lineTf->setText(stringForDurationEditorFromMinutes(p->getAdditional(Tf)));
    ui->lineZf->setText(stringForDurationEditorFromMinutes(p->getAdditional(Zf)));
    ui->lineZub->setText(stringForDurationEditorFromMinutes(p->getAdditional(Zub)));
    ui->lineService->setText(stringForDurationEditorFromMinutes(p->getAdditional(Service)));
    ui->lineZugVorbereiten->setText(stringForDurationEditorFromMinutes(p->getAdditional(ZugVorbereiten)));
    ui->lineWerkstatt->setText(stringForDurationEditorFromMinutes(p->getAdditional(Werkstatt)));
    ui->lineBuero->setText(stringForDurationEditorFromMinutes(p->getAdditional(Buero)));
    ui->lineAusbildung->setText(stringForDurationEditorFromMinutes(p->getAdditional(Ausbildung)));
    ui->lineInfrastruktur->setText(stringForDurationEditorFromMinutes(p->getAdditional(Infrastruktur)));
    ui->lineSonstiges->setText(stringForDurationEditorFromMinutes(p->getAdditional(Sonstiges)));
    ui->doubleAnzahl->setValue(p->getAdditional(Anzahl));
    ui->doubleKilometer->setValue(p->getAdditional(Kilometer));

    if (manager->getMinimumHours(Tf, p) > 0)
        ui->labelMinTf->setText(manager->getMinimumHoursString(Tf));
    if (manager->getMinimumHours(Zf, p) > 0)
        ui->labelMinZf->setText(manager->getMinimumHoursString(Zf));
    if (manager->getMinimumHours(Zub, p) > 0)
        ui->labelMinZub->setText(manager->getMinimumHoursString(Zub));
    if (manager->getMinimumHours(Service, p) > 0)
        ui->labelMinService->setText(manager->getMinimumHoursString(Service));
    if (manager->getMinimumHours(ZugVorbereiten, p) > 0)
        ui->labelMinZugVorbereiten->setText(manager->getMinimumHoursString(ZugVorbereiten));
    if (manager->getMinimumHours(Werkstatt, p) > 0)
        ui->labelMinWerkstatt->setText(manager->getMinimumHoursString(Werkstatt));
    if (manager->getMinimumHours(Buero, p) > 0)
        ui->labelMinBuero->setText(manager->getMinimumHoursString(Buero));
    if (manager->getMinimumHours(Ausbildung, p) > 0)
        ui->labelMinAusbildung->setText(manager->getMinimumHoursString(Ausbildung));
    if (manager->getMinimumHours(Infrastruktur, p) > 0)
        ui->labelMinInfrastruktur->setText(manager->getMinimumHoursString(Infrastruktur));
    if (manager->getMinimumHours(Sonstiges, p) > 0)
        ui->labelMinSonstiges->setText(manager->getMinimumHoursString(Sonstiges));

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
    on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
}

void PersonalWindow::on_pushAktualisieren_clicked()
{
    on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
    refreshGesamt();
}

void PersonalWindow::refreshGesamt()
{
    int origSort = ui->tabelleGesamt->horizontalHeader()->sortIndicatorSection();
    int newSort = 0;
    Qt::SortOrder sortOrder = ui->tabelleGesamt->horizontalHeader()->sortIndicatorOrder();
    QString origSortName = "";
    QTableWidgetItem *view = (ui->tabelleGesamt->horizontalHeaderItem(origSort));
    if (view != nullptr)
        origSortName = view->text();
    while(ui->tabelleGesamt->rowCount() > 0) {
        ui->tabelleGesamt->removeRow(0);
    }
    ui->tabelleGesamt->setSortingEnabled(false);

    // Alte Spalten entfernen und neue einfuegen
    while (ui->tabelleGesamt->columnCount() > 2) {
        ui->tabelleGesamt->removeColumn(2);
    }

    for(int i = anzeigeReihenfolge.length()-1; i >= 0; --i) {
        Category curr = anzeigeReihenfolge.at(i);
        if (! anzeige->contains(curr)) continue;
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(getLocalizedStringFromCategory(curr)));
        if (origSortName == getLocalizedStringFromCategory(curr)) newSort = 2;
    }
    ui->tabelleGesamt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    // Zeile für "Gesamt" einfuegen
    manager->berechne();

    ui->tabelleGesamt->insertRow(0);
    QTableWidgetItem *ii = new QTableWidgetItem();
    ui->tabelleGesamt->setItem(0, 0, ii);

    ii = new QTableWidgetItem(" Gesamt");
    ui->tabelleGesamt->setItem(0, 1, ii);

    int pos = 2;
    foreach(Category cat, anzeigeReihenfolge) {
        if (! anzeige->contains(cat)) continue;
        ii = new QTableWidgetItem();
        ii->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        switch(cat) {
        case Anzahl:
        case Kilometer:
            ii->setData(Qt::EditRole, manager->getTime(cat));
            break;
        default:
            ii->setData(Qt::EditRole, round(manager->getTime(cat)*100.f/60.f)/100);
        }

        ii->setData(Qt::EditRole, round(manager->getTime(cat)*100.f/60.f)/100);
        ui->tabelleGesamt->setItem(0, pos++, ii);
    }


    // Einzelne Personen einfuegen
    foreach (Person *p, current) {
        QString defaultFarbe = "#ffffff";
        QString farbe = defaultFarbe;
        switch (manager->pruefeStunden(p)) {
        case 0:
            farbe = nichtGenugStunden;
            break;
        case -1:
            farbe = genugStunden;
            break;
        default:
            farbe = "#ffffff";
        }
        ui->tabelleGesamt->insertRow(0);
        QTableWidgetItem *i = new QTableWidgetItem(p->getVorname());
        i->setBackground(QBrush(QColor(farbe)));
        ui->tabelleGesamt->setItem(0, 0, i);

        i = new QTableWidgetItem(p->getNachname());
        i->setBackground(QBrush(QColor(farbe)));
        ui->tabelleGesamt->setItem(0, 1, i);

        pos = 2;
        foreach(Category cat, anzeigeReihenfolge) {
            if (! anzeige->contains(cat)) continue;
            i = new QTableWidgetItem();
            i->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            i->setData(Qt::EditRole, round(p->get(cat)*100.f/60.f)/100);
            if (p->getAktiv())
                i->setBackground(QBrush(QColor(manager->checkHours(p, cat) ? defaultFarbe : nichtGenugStunden)));
            else
                i->setBackground(QBrush(QColor(defaultFarbe)));
            ui->tabelleGesamt->setItem(0, pos++, i);
        }
    }
    if (origSort < 2) {
        newSort = origSort;
    }
    ui->tabelleGesamt->sortByColumn(newSort, sortOrder);
    ui->tabelleGesamt->setSortingEnabled(true);
}

void PersonalWindow::refreshEinzel()
{
    ui->listWidget->clear();
    itemToPerson.clear();
    personToItem.clear();
    foreach (Person *p, current) {
        QListWidgetItem *item = new QListWidgetItem(p->getName());
        switch (manager->pruefeStunden(p)) {
        case 0:
            item->setBackground(QBrush(QColor(nichtGenugStunden)));
            break;
        case -1:
            item->setBackground(QBrush(QColor(genugStunden)));
            break;
        default:
            item->setBackground(QBrush(QColor("#ffffff")));
        }
        ui->listWidget->insertItem(0, item);
        itemToPerson.insert(item, p);
        personToItem.insert(p, item);
    }
    ui->listWidget->sortItems();
}

void PersonalWindow::on_pushAdd_clicked()
{
    Person *p = manager->newPerson();
    if (p == nullptr) {
        return;
    }
    aktuellePerson = p;
    QListWidgetItem *item = new QListWidgetItem(aktuellePerson->getName());
    ui->listWidget->insertItem(0, item);
    itemToPerson.insert(item, aktuellePerson);
    personToItem.insert(aktuellePerson, item);
    showPerson(aktuellePerson);
    ui->pushDelete->setEnabled(true);
    refreshEinzel();
    emit changed();
    on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
}

void PersonalWindow::on_lineVorname_textChanged(const QString &arg1)
{
    if (enabled) {
        // test, ob Person vorhanden ist
        if (manager->personExists(arg1, aktuellePerson->getNachname())) {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        } else {
            aktuellePerson->setVorname(arg1);
            if (personToItem.contains(aktuellePerson)) {
                personToItem.value(aktuellePerson)->setText(aktuellePerson->getName());
                ui->listWidget->sortItems();
            }
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
            if (personToItem.contains(aktuellePerson)) {
                personToItem.value(aktuellePerson)->setText(aktuellePerson->getName());
                ui->listWidget->sortItems();
            }
        }
        emit changed();
    }
}

void PersonalWindow::on_spinKm_valueChanged(int arg1)
{
    if (enabled) {
        aktuellePerson->setStrecke(arg1);
        ui->labelKilometerSum->setText(QString::number(aktuellePerson->get(Kilometer)));
        emit changed();
    }
}

void PersonalWindow::on_checkTf_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungTf(checked);
        on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
        emit changed();
    }
}

void PersonalWindow::on_checkZf_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungZf(checked);
        on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
        emit changed();
    }
}

void PersonalWindow::on_checkRangierer_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungRangierer(checked);
        on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
        emit changed();
    }
}

void PersonalWindow::on_pushDelete_clicked()
{
    // Lösche die Person
    if (enabled) {
        enabled = false;
        Person *p = aktuellePerson;
        QListWidgetItem *i = personToItem.value(p);
        if (p->getAnzahl() > 0) {
            QMessageBox::information(this, tr("Warnung"), tr("Die ausgewählte Person kann nicht gelöscht werden, da Sie noch bei Aktivitäten eingetragen ist.\nBitte lösen Sie diese Verbindung bevor Sie die Person löschen!"));
            enabled = true;
            return;
        }
        if (p == aktuellePerson) {
            aktuellePerson = nullptr;
        }
        itemToPerson.remove(i);
        personToItem.remove(p);
        manager->removePerson(p);
        delete p;
        ui->listWidget->takeItem(ui->listWidget->row(i));

        if (ui->listWidget->count() == 0) {
            toggleFields(false);
            ui->pushDelete->setEnabled(false);
        } else {
            aktuellePerson = itemToPerson.value(ui->listWidget->item(0));
            showPerson(aktuellePerson);
            enabled = true;
        }
        emit changed();
    }
}

void PersonalWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    showPerson(itemToPerson.value(item));
}

void PersonalWindow::on_pushPDF_clicked()
{
    QPrinter *pdf = nullptr;
    pdf = Export::getPrinterPDF(this, "Personal-Gesamt.pdf");
    print(pdf);
}

void PersonalWindow::on_pushPrint_clicked()
{
    QPrinter *paper = nullptr;
    paper = Export::getPrinterPaper(this);
    print(paper);
}

void PersonalWindow::on_actionSinglePrint_triggered()
{
    QPrinter *paper = nullptr;
    paper = Export::getPrinterPaper(this);
    Export::printPerson(manager, aktuellePerson, paper);
}

void PersonalWindow::on_actionSinglePDF_triggered()
{
    QPrinter *pdf = nullptr;
    pdf = Export::getPrinterPDF(this, "Personal-Einzelansicht.pdf");
    Export::printPerson(manager, aktuellePerson, pdf);
}

void PersonalWindow::on_pushPDFEinzel_clicked()
{
    QPrinter *pdf = nullptr;
    pdf = Export::getPrinterPDF(this, "Personal-Einzelansicht.pdf");
    Export::printPerson(manager, pdf);
}

void PersonalWindow::on_pushPrintEinzel_clicked()
{
    QPrinter *paper = nullptr;
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
    QMap<Category, int> gesamt = QMap<Category, int>();
    gesamt.insert(Category::Gesamt, manager->getTime(Gesamt));
    gesamt.insert(Category::Anzahl, 0);
    gesamt.insert(Category::Tf, manager->getTime(Tf));
    gesamt.insert(Category::Zf, manager->getTime(Zf));
    gesamt.insert(Category::Zub, manager->getTime(Zub));
    gesamt.insert(Category::Service, manager->getTime(Service));
    gesamt.insert(Category::ZugVorbereiten, manager->getTime(ZugVorbereiten));
    gesamt.insert(Category::Werkstatt, manager->getTime(Werkstatt));
    gesamt.insert(Category::Buero, manager->getTime(Buero));
    gesamt.insert(Category::Ausbildung, manager->getTime(Ausbildung));
    gesamt.insert(Category::Infrastruktur, manager->getTime(Infrastruktur));
    gesamt.insert(Category::Sonstiges, manager->getTime(Sonstiges));
    gesamt.insert(Category::Kilometer, manager->getTime(Kilometer));
    QList<Category> anzeigeListe = QList<Category>();
    foreach(Category cat, anzeigeReihenfolge) {
        if (anzeige->contains(cat)) anzeigeListe.append(cat);
    }
    Export::printPersonen(liste, gesamt, anzeigeListe, p);
}

void PersonalWindow::toggleFields(bool state)
{
    ui->lineID->setEnabled(state);
    ui->pushAutoID->setEnabled(state);
    ui->lineVorname->setEnabled(state);
    ui->lineNachname->setEnabled(state);
    ui->dateBirth->setEnabled(state);
    ui->dateEntry->setEnabled(state);
    ui->checkAktiv->setEnabled(state);

    ui->checkRangierer->setEnabled(state);
    ui->checkTf->setEnabled(state);
    ui->checkZf->setEnabled(state);
    ui->dateDienst->setEnabled(state);

    ui->lineMail->setEnabled(state);
    ui->checkMail->setEnabled(state);
    ui->linePhone->setEnabled(state);
    ui->checkPhone->setEnabled(state);

    ui->spinKm->setEnabled(state);
    ui->lineJob->setEnabled(state);
    ui->plainBemerkung->setEnabled(state);
    ui->dateExit->setEnabled(state);
    ui->pushDelete->setEnabled(state);

    ui->tabelle->setEnabled(state);

    ui->lineTf->setEnabled(state);
    ui->lineZf->setEnabled(state);
    ui->lineZub->setEnabled(state);
    ui->lineService->setEnabled(state);
    ui->lineZugVorbereiten->setEnabled(state);
    ui->lineWerkstatt->setEnabled(state);
    ui->lineBuero->setEnabled(state);
    ui->lineAusbildung->setEnabled(state);
    ui->lineInfrastruktur->setEnabled(state);
    ui->lineSonstiges->setEnabled(state);
    ui->doubleAnzahl->setEnabled(state);
    ui->doubleKilometer->setEnabled(state);

}

void PersonalWindow::updateZeiten()
{
    on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
    ui->labelTfSum->setText(aktuellePerson->getString(Tf));
    ui->labelTfSum->update();
    ui->labelZfSum->setText(aktuellePerson->getString(Zf));
    ui->labelZfSum->update();
    ui->labelZubSum->setText(aktuellePerson->getString(Zub));
    ui->labelZubSum->update();
    ui->labelServiceSum->setText(aktuellePerson->getString(Service));
    ui->labelServiceSum->update();
    ui->labelZugVorbereitenSum->setText(aktuellePerson->getString(ZugVorbereiten));
    ui->labelZugVorbereitenSum->update();
    ui->labelWerkstattSum->setText(aktuellePerson->getString(Werkstatt));
    ui->labelWerkstattSum->update();
    ui->labelBueroSum->setText(aktuellePerson->getString(Buero));
    ui->labelBueroSum->update();
    ui->labelAusbildungSum->setText(aktuellePerson->getString(Ausbildung));
    ui->labelAusbildungSum->update();
    ui->labelInfrastrukturSum->setText(aktuellePerson->getString(Infrastruktur));
    ui->labelInfrastrukturSum->update();
    ui->labelSonstigesSum->setText(aktuellePerson->getString(Sonstiges));
    ui->labelSonstigesSum->update();
    ui->labelAnzahlSum->setText(aktuellePerson->getString(Anzahl));
    ui->labelAnzahlSum->update();
    ui->labelKilometerSum->setText(aktuellePerson->getString(Kilometer));
    ui->labelKilometerSum->update();
    ui->labelGesamt->setText(aktuellePerson->getString(Gesamt));
    ui->labelGesamt->update();
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
    if (checked) anzeige->insert(Category::Gesamt);
    else anzeige->remove(Category::Gesamt);
    refreshGesamt();
}

void PersonalWindow::on_checkShowAnzahl_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Anzahl);
    else anzeige->remove(Category::Anzahl);
    refreshGesamt();
}

void PersonalWindow::on_checkShowTf_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Tf);
    else anzeige->remove(Category::Tf);
    refreshGesamt();
}

void PersonalWindow::on_checkShowZf_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Zf);
    else anzeige->remove(Category::Zf);
    refreshGesamt();
}

void PersonalWindow::on_checkShowZub_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Zub);
    else anzeige->remove(Category::Zub);
    refreshGesamt();
}

void PersonalWindow::on_checkShowService_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Service);
    else anzeige->remove(Category::Service);
    refreshGesamt();
}

void PersonalWindow::on_checkShowVorbereiten_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::ZugVorbereiten);
    else anzeige->remove(Category::ZugVorbereiten);
    refreshGesamt();
}

void PersonalWindow::on_checkShowWerkstatt_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Werkstatt);
    else anzeige->remove(Category::Werkstatt);
    refreshGesamt();
}

void PersonalWindow::on_checkShowBuero_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Buero);
    else anzeige->remove(Category::Buero);
    refreshGesamt();
}

void PersonalWindow::on_checkShowAusbildung_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Ausbildung);
    else anzeige->remove(Category::Ausbildung);
    refreshGesamt();
}

void PersonalWindow::on_checkShowInfrastruktur_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Infrastruktur);
    else anzeige->remove(Category::Infrastruktur);
    refreshGesamt();
}

void PersonalWindow::on_checkShowSonstiges_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Sonstiges);
    else anzeige->remove(Category::Sonstiges);
    refreshGesamt();
}

void PersonalWindow::on_checkShowKilometer_clicked(bool checked)
{
    if (checked) anzeige->insert(Category::Kilometer);
    else anzeige->remove(Category::Kilometer);
    refreshGesamt();
}

void PersonalWindow::on_lineTf_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Tf, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineZf_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Zf, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineZub_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Zub, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineService_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Service, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineZugVorbereiten_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(ZugVorbereiten, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineWerkstatt_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Werkstatt, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineBuero_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Buero, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineAusbildung_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Ausbildung, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineInfrastruktur_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Infrastruktur, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_lineSonstiges_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Sonstiges, minutesFromStringForDurationEditor(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_doubleAnzahl_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Anzahl, int(arg1));
        updateZeiten();
        emit changed();
    }
}

void PersonalWindow::on_doubleKilometer_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Kilometer, int(arg1));
        updateZeiten();
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

void PersonalWindow::on_tabelleGesamt_cellDoubleClicked(int row, int column)
{
    QString name = ui->tabelleGesamt->item(row, 0)->text() + " " + ui->tabelleGesamt->item(row, 1)->text();
    Person * p = manager->getPerson(name);
    if (p != nullptr) {
        showPerson(p);
        ui->tabWidgetMain->setCurrentIndex(1);
    }
}

void PersonalWindow::on_lineID_textChanged(const QString &arg1)
{
    if (enabled) {
        if (arg1.toInt() > 0) {
            if (aktuellePerson->setNummer(arg1.toInt())) {
                emit changed();
            } else {
                QMessageBox::information(this, tr("Fehler"), tr("Die Mitgliedsnummer konnte nicht geändert werden, da sie bereits vergeben ist. Bitte wählen Sie eine andere Nummer!"));
            }
        }
    }
}

void PersonalWindow::on_dateBirth_dateChanged(const QDate &date)
{
    if (enabled) {
        aktuellePerson->setGeburtstag(date);
        emit changed();
    }
}

void PersonalWindow::on_dateEntry_dateChanged(const QDate &date)
{
    if (enabled) {
        aktuellePerson->setEintritt(date);
        emit changed();
    }
}

void PersonalWindow::on_lineJob_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setBeruf(arg1);
        emit changed();
    }
}

void PersonalWindow::on_checkAktiv_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAktiv(checked);
        on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
        emit changed();
    }
}

void PersonalWindow::on_linePhone_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setTelefon(arg1);
        emit changed();
    }
}

void PersonalWindow::on_checkPhone_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setTelefonOK(checked);
        emit changed();
    }
}

void PersonalWindow::on_lineMail_textChanged(const QString &arg1)
{
    if (enabled) {
        aktuellePerson->setMail(arg1);
        emit changed();
    }
}

void PersonalWindow::on_checkMail_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setMailOK(checked);
        emit changed();
    }
}

void PersonalWindow::on_dateDienst_dateChanged(const QDate &date)
{
    if (enabled) {
        aktuellePerson->setTauglichkeit(date);
        emit changed();
    }
}

void PersonalWindow::on_plainBemerkung_textChanged()
{
    if (enabled) {
        aktuellePerson->setBemerkungen(ui->plainBemerkung->toPlainText());
        emit changed();
    }
}

void PersonalWindow::on_dateExit_dateChanged(const QDate &date)
{
    if (enabled) {
        aktuellePerson->setAustritt(date);
        on_comboEinzel_currentIndexChanged(ui->comboEinzel->currentIndex());
        emit changed();
    }
}

void PersonalWindow::on_pushAutoID_clicked()
{
    if (enabled) {
        enabled=false;
        aktuellePerson->setNummer(manager->getNextNummer());
        ui->lineID->setText(QString::number(aktuellePerson->getNummer()));
        enabled=true;
    }
}

void PersonalWindow::on_comboEinzel_currentIndexChanged(int index)
{
    /* 0 Alle Mitglieder
     * 1 Aktiv
     * 2 Passiv
     * 3 Aktiv mit Stunden
     * 4 Aktiv ohne Stunden
     * 5 Passiv mit Stunden
     * 6 Ausgetreten
     * 7 Alle
     * */
    ui->comboGesamt->setCurrentIndex(index);

    current.clear();
    QListIterator<Person*> i = manager->getPersonen();
    while(i.hasNext()) {
        Person *p = i.next();
        switch (index) {
        case 0:
            if (! p->isAusgetreten()) current.append(p);
            break;
        case 1:
            if ((! p->isAusgetreten()) && p->getAktiv()) current.append(p);
            break;
        case 2:
            if ((! p->isAusgetreten()) && (! p->getAktiv())) current.append(p);
            break;
        case 3:
            if ((! p->isAusgetreten()) && (p->getAktiv()) && (manager->pruefeStunden(p) == 1)) current.append(p);
            break;
        case 4:
            if ((! p->isAusgetreten()) && (p->getAktiv()) && (manager->pruefeStunden(p) == 0)) current.append(p);
            break;
        case 5:
            if ((! p->isAusgetreten()) && (!p->getAktiv()) && (manager->pruefeStunden(p) == -1)) current.append(p);
            break;
        case 6:
            if (p->isAusgetreten()) current.append(p);
            break;
        default:
            current.append(p);
            break;
        }
    }
    refresh();
}

void PersonalWindow::on_comboGesamt_currentIndexChanged(int index)
{
    ui->comboEinzel->setCurrentIndex(index);
    on_comboEinzel_currentIndexChanged(index);
}

void PersonalWindow::on_pushEmail_clicked()
{
    QString s = "mailto:?bcc=";
    QStringList mails;
    foreach (Person *p, current) {
        mails.append(p->getMail());
    }
    s += mails.join(",");
    QDesktopServices::openUrl(QUrl(s));
    //"mailto:info-gs@ostertalbahn.de?cc=ako@ostertalbahn.de&bcc=philjosch@t-online.de,philipp.hio@t-online.de"));

}
