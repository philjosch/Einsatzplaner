#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include "export.h"
#include "minimumhourseditordialog.h"

#include <QMessageBox>

const QString PersonalWindow::nichtGenugStunden = "#ff9999";
const QList<Category> PersonalWindow::anzeigeReihenfolge = {Category::Gesamt, Category::Anzahl, Category::Tf, Category::Zf, Category::Zub, Category::Service, Category::ZugVorbereiten, Category::Werkstatt, Category::Buero, Category::Ausbildung, Category::Sonstiges, Category::Kilometer};

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

    QSetIterator<Person*> i = manager->getPersonen();
    while(i.hasNext()) {
        Person *p = i.next();
        QListWidgetItem *item = new QListWidgetItem(p->getName());
        ui->listWidget->insertItem(0, item);
        itemToPerson.insert(item, p);
        personToItem.insert(p, item);
        ui->pushDelete->setEnabled(true);
    }
    refreshEinzel();

    anzeige = new QSet<Category>();
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
    ui->doubleAnzahl->setEnabled(true);
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

        AActivity::Infos infos = a->getIndividual(p);

        // Aufgabe
        QTableWidgetItem *i1 = new QTableWidgetItem(AActivity::getStringFromCategory(infos.kategorie));
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

    ui->lineTf->setText(QString::number(p->getTimeTf()));
    ui->lineZf->setText(QString::number(p->getTimeZf()));
    ui->lineZub->setText(QString::number(p->getTimeZub()));
    ui->lineService->setText(QString::number(p->getTimeService()));
    ui->lineZugVorbereiten->setText(QString::number(p->getTimeVorbereiten()));
    ui->lineWerkstatt->setText(QString::number(p->getTimeWerkstatt()));
    ui->lineBuero->setText(QString::number(p->getTimeBuero()));
    ui->lineAusbildung->setText(QString::number(p->getTimeAusbildung()));
    ui->lineSonstiges->setText(QString::number(p->getTimeSonstiges()));
    ui->lineAnzahl->setText(QString::number(p->getSumAnzahl()));
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
    ui->doubleAnzahl->setValue(p->getAdditionalAnzahl());
    ui->doubleKilometer->setValue(p->getAdditionalKilometer());

    if (p->getAusbildungTf() && manager->getMinimumHours(Tf) > 0)
        ui->lineMinTf->setText(QString::number(manager->getMinimumHours(Tf)));
    if (p->getAusbildungZf() && manager->getMinimumHours(Zf) > 0)
        ui->lineMinZf->setText(QString::number(manager->getMinimumHours(Zf)));
    if (manager->getMinimumHours(Zub) > 0)
        ui->lineMinZub->setText(QString::number(manager->getMinimumHours(Zub)));
    if (manager->getMinimumHours(Service) > 0)
        ui->lineMinService->setText(QString::number(manager->getMinimumHours(Service)));
    if (manager->getMinimumHours(ZugVorbereiten) > 0)
        ui->lineMinZugVorbereiten->setText(QString::number(manager->getMinimumHours(ZugVorbereiten)));
    if (manager->getMinimumHours(Werkstatt) > 0)
        ui->lineMinWerkstatt->setText(QString::number(manager->getMinimumHours(Werkstatt)));
    if (manager->getMinimumHours(Buero) > 0)
        ui->lineMinBuero->setText(QString::number(manager->getMinimumHours(Buero)));
    if ((p->getAusbildungTf() || p->getAusbildungZf() || p->getAusbildungRangierer())
            && manager->getMinimumHours(Ausbildung) > 0)
        ui->lineMinAusbildung->setText(QString::number(manager->getMinimumHours(Ausbildung)));
    if (manager->getMinimumHours(Sonstiges) > 0)
        ui->lineMinSonstiges->setText(QString::number(manager->getMinimumHours(Sonstiges)));

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
        itemToPerson.insert(item, p);
        personToItem.insert(p, item);
    }
    refresh();
}

void PersonalWindow::on_pushAktualisieren_clicked()
{
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

    QSetIterator<Person*> iterator = manager->getPersonen();
    //  0: summe gesamt
    //  1: anzahl
    //  2: tf/tb
    //  3: zf
    //  4: zub/begl.o.b.a.
    //  5: service
    //  6: zug vorbereiten
    //  7: werkstatt
    //  8: büro
    //  9: ausbildung
    // 10: sonstiges
    // 11: kilometer
    while (ui->tabelleGesamt->columnCount() > 2) {
        ui->tabelleGesamt->removeColumn(2);
    }

    if (anzeige->contains(Category::Kilometer)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Kilometer")));
        if (origSortName == tr("Kilometer")) newSort = 2;
    }
    if (anzeige->contains(Category::Sonstiges)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Sonstiges")));
        if (origSortName == tr("Sonstiges")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Ausbildung)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Ausbildung")));
        if (origSortName == tr("Ausbildung")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Buero)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Büro")));
        if (origSortName == tr("Büro")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Werkstatt)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Werkstatt")));
        if (origSortName == tr("Werkstatt")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::ZugVorbereiten)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Zug\nVorbereiten")));
        if (origSortName == tr("Zug\nVorbereiten")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Service)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Service")));
        if (origSortName == tr("Service")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Zub)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Zub")));
        if (origSortName == tr("Zub")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Zf)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Zf")));
        if (origSortName == tr("Zf")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Tf)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Tf/Tb")));
        if (origSortName == tr("Tf/Tb")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Anzahl)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Anzahl")));
        if (origSortName == tr("Anzahl")) newSort = 2; else newSort++;
    }
    if (anzeige->contains(Category::Gesamt)) {
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Gesamt")));
        if (origSortName == tr("Gesamt")) newSort = 2; else newSort++;
    }
    ui->tabelleGesamt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Insert the sum of each column
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
        ii->setData(Qt::EditRole, manager->getTime(cat));
        ui->tabelleGesamt->setItem(0, pos++, ii);
    }
    // End of insert the sum of each column


    while(iterator.hasNext()) {
        Person *p = iterator.next();
        QString defaultFarbe = "#ffffff";
        QString farbe = defaultFarbe;
        if (! manager->pruefeStunden(p)) {
            farbe = nichtGenugStunden;
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
            i->setData(Qt::EditRole, p->getTime(cat));
            i->setBackground(QBrush(QColor(manager->checkHours(p, cat) ? defaultFarbe : nichtGenugStunden)));
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
    // hier müssen nur die Farben angepasst werden
    for(int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *item = ui->listWidget->item(i);
        Person *p = itemToPerson.value(item);
        if (manager->pruefeStunden(p)) {
            item->setBackground(QBrush(QColor("#ffffff")));
        } else {
            item->setBackground(QBrush(QColor(nichtGenugStunden)));
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
    itemToPerson.insert(item, aktuellePerson);
    personToItem.insert(aktuellePerson, item);
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
            personToItem.value(aktuellePerson)->setText(aktuellePerson->getName());
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
            personToItem.value(aktuellePerson)->setText(aktuellePerson->getName());
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
        Person *p = itemToPerson.value(i);
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
            disableFields();
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
    QMap<Category, double> gesamt = QMap<Category, double>();
    gesamt.insert(Category::Gesamt, manager->getTimeSum());
    gesamt.insert(Category::Anzahl, 0);
    gesamt.insert(Category::Tf, manager->getTimeTf());
    gesamt.insert(Category::Zf, manager->getTimeZf());
    gesamt.insert(Category::Zub, manager->getTimeZub());
    gesamt.insert(Category::Service, manager->getTimeService());
    gesamt.insert(Category::ZugVorbereiten, manager->getTimeVorbereiten());
    gesamt.insert(Category::Werkstatt, manager->getTimeWerkstatt());
    gesamt.insert(Category::Buero, manager->getTimeBuero());
    gesamt.insert(Category::Ausbildung, manager->getTimeAusbildung());
    gesamt.insert(Category::Sonstiges, manager->getTimeSonstiges());
    gesamt.insert(Category::Kilometer, manager->getSumKilometer());
    QList<Category> anzeigeListe = QList<Category>();
    foreach(Category cat, anzeigeReihenfolge) {
        if (anzeige->contains(cat)) anzeigeListe.append(cat);
    }
    Export::printPersonen(liste, gesamt, anzeigeListe, p);
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
    ui->doubleAnzahl->setEnabled(false);
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
    ui->lineAnzahl->setEnabled(false);
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

void PersonalWindow::on_doubleAnzahl_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditionalAnzahl(int(arg1));
        ui->lineAnzahl->setText(QString::number(aktuellePerson->getSumAnzahl()));
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
