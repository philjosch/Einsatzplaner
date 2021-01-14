#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include "export.h"
#include "minimumhourseditordialog.h"
#include "fileio.h"
#include "guihelper.h"
#include "einstellungen.h"

#include <QMessageBox>
#include <math.h>
#include <cmath>
#include <QDesktopServices>

PersonalWindow::PersonalWindow(QWidget *parent, ManagerPersonal *m) : QMainWindow(parent), ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);
    connect(ui->actionMindeststunden, SIGNAL(triggered()), this, SLOT(editMinimumHours()));
    connect(ui->comboAnzeige, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
    connect(ui->actionAktualisieren, SIGNAL(triggered()), this, SLOT(refresh()));

    // Initalisieren der Statischen variablen
    manager = m;
    setWindowTitle(tr("Mitgliederverwaltung"));

    personToItem = QHash<Person*, QListWidgetItem*>();
    enabled = false;

    current = QList<Person*>();
    filter = Mitglied::Aktiv;

    anzeige = QSet<Category>();
    ui->checkShowGesamt->setChecked(true);
    ui->checkShowAnzahl->setChecked(true);
    ui->checkShowKilometer->setChecked(true);
    on_checkShowGesamt_clicked(true);
    on_checkShowAnzahl_clicked(true);
    on_checkShowKilometer_clicked(true);
    ui->comboAnzeige->setCurrentIndex(1);
    ui->tabelleGesamt->sortItems(1);
    aktuellePerson = nullptr;
    refresh();

}

PersonalWindow::~PersonalWindow()
{
    delete ui;
}

void PersonalWindow::refresh()
{
    switch (ui->comboAnzeige->currentIndex()) {
    case 0: filter = AlleMitglieder; break;
    case 1: filter = Aktiv; break;
    case 2: filter = Passiv; break;
    case 3: filter = AktivMit; break;
    case 4: filter = AktivOhne; break;
    case 5: filter = PassivMit; break;
    case 6: filter = Ausgetreten; break;
    default: filter = Registriert; break;
    }
    current = manager->getPersonen(filter);
    // Aktualisiere die Ansichten
    refreshEinsatzzeiten();
    refreshEinzel();
}
void PersonalWindow::refreshEinsatzzeiten()
{
    // Alte Spalte der Sortierung bestimen
    Qt::SortOrder sortOrder = ui->tabelleGesamt->horizontalHeader()->sortIndicatorOrder();
    int origSort = ui->tabelleGesamt->horizontalHeader()->sortIndicatorSection();
    int newSort = 0;
    QString origSortName = "";
    QTableWidgetItem *view = (ui->tabelleGesamt->horizontalHeaderItem(origSort));
    if (view != nullptr)
        origSortName = view->text();

    // Tabelle leeren
    ui->tabelleGesamt->setRowCount(0);
    ui->tabelleGesamt->setColumnCount(2);
    ui->tabelleGesamt->setSortingEnabled(false);

    // Neue Spalten einfuegen
    for(int i = ANZEIGEREIHENFOLGEGESAMT.length()-1; i >= 0; --i) {
        Category curr = ANZEIGEREIHENFOLGEGESAMT.at(i);
        if (! anzeige.contains(curr)) continue;
        ui->tabelleGesamt->insertColumn(2);
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(getLocalizedStringFromCategory(curr)));
        if (origSortName == getLocalizedStringFromCategory(curr)) newSort = 2;
        else newSort++;
    }
    ui->tabelleGesamt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    // Zeile für "Gesamt" einfuegen
    manager->berechne();



    // Einzelne Personen einfuegen und Summe berechnen
    int pos;
    QMap<Category, int> sum;
    for (Person *p: current) {
        QString farbe = Person::FARBE_STANDARD;
        switch (p->pruefeStunden()) {
        case AktivOhne:  farbe = Person::FARBE_FEHLENDE_STUNDEN; break;
        case PassivMit: farbe = Person::FARBE_GENUG_STUNDEN; break;
        default: farbe = Person::FARBE_STANDARD;
        }
        ui->tabelleGesamt->insertRow(0);
        QTableWidgetItem *i = new QTableWidgetItem(p->getVorname());
        i->setBackground(QBrush(QColor(farbe)));
        ui->tabelleGesamt->setItem(0, 0, i);

        i = new QTableWidgetItem(p->getNachname());
        i->setBackground(QBrush(QColor(farbe)));
        ui->tabelleGesamt->setItem(0, 1, i);

        pos = 2;
        for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
            if (! anzeige.contains(cat)) continue;
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
            i = new QTableWidgetItem();
            i->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            switch (cat) {
            case Anzahl:
            case Kilometer:
                i->setData(Qt::EditRole, p->getZeiten(cat));
                break;
            default:
                i->setData(Qt::EditRole, round(p->getZeiten(cat)*100.f/60.f)/100);
            }

            switch (p->pruefeStunden(cat)) {
            case AktivOhne:  i->setBackground(QBrush(QColor(Person::FARBE_FEHLENDE_STUNDEN))); break;
            case PassivMit: i->setBackground(QBrush(QColor(Person::FARBE_GENUG_STUNDEN))); break;
            default: i->setBackground(QBrush(QColor(Person::FARBE_STANDARD)));
            }
            ui->tabelleGesamt->setItem(0, pos++, i);
        }
    }

    // Zeile Gesamt einfuegen
    ui->tabelleGesamt->insertRow(0);
    ui->tabelleGesamt->setItem(0, 0, new QTableWidgetItem());
    ui->tabelleGesamt->setItem(0, 1, new QTableWidgetItem(tr(" Gesamt")));
    pos = 2;
    QTableWidgetItem *ii;
    for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        if (! anzeige.contains(cat)) continue;
        ii = new QTableWidgetItem();
        ii->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        switch(cat) {
        case Anzahl:
        case Kilometer:
            ii->setData(Qt::EditRole, sum.value(cat));
            break;
        default:
            ii->setData(Qt::EditRole, round(sum.value(cat)*100.f/60.f)/100);
        }
        ui->tabelleGesamt->setItem(0, pos++, ii);
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
    personToItem.clear();
    for (Person *p: current) {
        QListWidgetItem *item = new QListWidgetItem(p->getName());
        if (! Einstellungen::getReihenfolgeVorNach()) {
            item->setText(
                        p->getNachname() + ", " + p->getVorname());
        }
        switch (p->pruefeStunden()) {
        case AktivOhne:
            item->setBackground(QBrush(QColor(Person::FARBE_FEHLENDE_STUNDEN)));
            break;
        case PassivMit:
            item->setBackground(QBrush(QColor(Person::FARBE_GENUG_STUNDEN)));
            break;
        default:
            item->setBackground(QBrush(QColor(Person::FARBE_STANDARD)));
        }
        ui->listWidget->insertItem(0, item);
        personToItem.insert(p, item);
    }
    ui->listWidget->sortItems();
}

void PersonalWindow::editMinimumHours()
{
   if (MinimumHoursEditorDialog(manager, this).exec() == QDialog::Accepted) {
       refresh();
   }
}

void PersonalWindow::on_actionZeitenEinzelEinzelPDF_triggered()
{
    Export::Personal::printZeitenEinzelEinzel(aktuellePerson,
                        Export::getPrinterPDF(this, "Einsatzzeiten-Einzelansicht", QPrinter::Orientation::Portrait));
}
void PersonalWindow::on_actionZeitenEinzelEinzelDrucken_triggered()
{
    Export::Personal::printZeitenEinzelEinzel(aktuellePerson,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}

void PersonalWindow::on_actionZeitenEinzelListePDF_triggered()
{
    Export::Personal::printZeitenEinzelListe(getSortierteListe(), manager, filter,
                        Export::getPrinterPDF(this, "Einsatzzeiten-Einzelansichten", QPrinter::Orientation::Portrait));
}
void PersonalWindow::on_actionZeitenEinzelListeDrucken_triggered()
{
    Export::Personal::printZeitenEinzelListe(getSortierteListe(), manager, filter,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}

void PersonalWindow::on_actionZeitenListePDF_triggered()
{
    Export::Personal::printZeitenListe(
                getSortierteListe(), anzeige, filter,
                Export::getPrinterPDF(this, "Einsatzzeiten-Gesamt", QPrinter::Orientation::Landscape));
}
void PersonalWindow::on_actionZeitenListeDrucken_triggered()
{
    Export::Personal::printZeitenListe(
                getSortierteListe(), anzeige, filter,
                Export::getPrinterPaper(this, QPrinter::Orientation::Landscape));
}

void PersonalWindow::on_actionMitgliederEinzelEinzelPDF_triggered()
{
    Export::Mitglieder::printMitgliederEinzelEinzel(aktuellePerson,
                        Export::getPrinterPDF(this, "Stammdatenblatt", QPrinter::Orientation::Portrait));
}
void PersonalWindow::on_actionMitgliederEinzelEinzelDrucken_triggered()
{
    Export::Mitglieder::printMitgliederEinzelEinzel(aktuellePerson,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}

void PersonalWindow::on_actionMitgliederEinzelListePDF_triggered()
{
    Export::Mitglieder::printMitgliederEinzelListe(getSortierteListe(), manager, filter,
                        Export::getPrinterPDF(this, "Stammdatenblaetter", QPrinter::Orientation::Portrait));
}
void PersonalWindow::on_actionMitgliederEinzelListeDrucken_triggered()
{
    Export::Mitglieder::printMitgliederEinzelListe(getSortierteListe(), manager, filter,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}

void PersonalWindow::on_actionMitgliederListePDF_triggered()
{
    Export::Mitglieder::printMitgliederListe(getSortierteListe(), filter,
                            Export::getPrinterPDF(this, "Mitgliederliste", QPrinter::Orientation::Portrait));
}
void PersonalWindow::on_actionMitgliederListeDrucken_triggered()
{
    Export::Mitglieder::printMitgliederListe(getSortierteListe(), filter,
                            Export::getPrinterPaper(this, QPrinter::Orientation::Landscape));
}
void PersonalWindow::on_actionMitgliederListeCSV_triggered()
{
    Export::Mitglieder::exportMitgliederAlsCSV(current,
                                  FileIO::getFilePathSave(this, "Mitglieder", FileIO::DateiTyp::CSV));
}

void PersonalWindow::on_pushEmail_clicked()
{
    if (current.isEmpty()) return;
    QSet<QString> mails;
    QList<Person*> keineMail;
    for (Person *p: current) {
        if (p->getMail() != "") {
            mails.insert(p->getMail());
        } else {
            keineMail.append(p);
        }
    }
    if (! mails.isEmpty()) {
        QString betreff = "&subject=[%1]";
        if (filter == Mitglied::AlleMitglieder) {
            betreff = betreff.arg(tr("AkO-Alle"));
        } else if (filter == Mitglied::Aktiv) {
            betreff = betreff.arg(tr("AkO-Aktive"));
        } else if (filter == Mitglied::Passiv) {
            betreff = betreff.arg(tr("AkO-Passive"));
        } else {
            betreff = "";
        }
        QString s = "mailto:?bcc=";
        s += mails.values().join(",");
        s += betreff;
        QDesktopServices::openUrl(QUrl(s));
    }
    if (keineMail.isEmpty()) return;

    if (QMessageBox::question(this, tr("Personen ohne E-Mail gefunden"), tr("Für %1 Personen ist keine E-Mail-Adresse hinterlegt.\nMöchten Sie die Liste der Personen speichern?").arg(keineMail.size())) == QMessageBox::Yes) {
        QString s = "Name;Straße;PLZ;Ort\n";
        for(Person *p: keineMail) {
            s += p->getName() + ";"+p->getStrasse()+";"+p->getPLZ()+";"+p->getOrt()+"\n";
        }
        QString path = FileIO::getFilePathSave(this, "Adressen", FileIO::DateiTyp::CSV);
        FileIO::saveToFile(path, s);
    }
}

void PersonalWindow::on_tabelleGesamt_cellDoubleClicked(int row, [[maybe_unused]] int column)
{
    QString name = ui->tabelleGesamt->item(row, 0)->text() + " " + ui->tabelleGesamt->item(row, 1)->text();
    Person * p = manager->getPerson(name);
    if (p != nullptr) {
        showPerson(p);
        ui->tabWidgetMain->setCurrentIndex(1);
    }
}

void PersonalWindow::on_checkShowGesamt_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Gesamt);
    else anzeige.remove(Category::Gesamt);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowAnzahl_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Anzahl);
    else anzeige.remove(Category::Anzahl);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowTf_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Tf);
    else anzeige.remove(Category::Tf);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowZf_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Zf);
    else anzeige.remove(Category::Zf);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowZub_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Zub);
    else anzeige.remove(Category::Zub);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowService_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Service);
    else anzeige.remove(Category::Service);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowVorbereiten_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::ZugVorbereiten);
    else anzeige.remove(Category::ZugVorbereiten);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowWerkstatt_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Werkstatt);
    else anzeige.remove(Category::Werkstatt);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowBuero_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Buero);
    else anzeige.remove(Category::Buero);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowAusbildung_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Ausbildung);
    else anzeige.remove(Category::Ausbildung);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowInfrastruktur_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Infrastruktur);
    else anzeige.remove(Category::Infrastruktur);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowSonstiges_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Sonstiges);
    else anzeige.remove(Category::Sonstiges);
    refreshEinsatzzeiten();
}
void PersonalWindow::on_checkShowKilometer_clicked(bool checked)
{
    if (checked) anzeige.insert(Category::Kilometer);
    else anzeige.remove(Category::Kilometer);
    refreshEinsatzzeiten();
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
    personToItem.insert(aktuellePerson, item);
    showPerson(aktuellePerson);
    refresh();
}

void PersonalWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    showPerson(personToItem.key(item));
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
                if (Einstellungen::getReihenfolgeVorNach()) {
                    personToItem.value(aktuellePerson)->setText(aktuellePerson->getName());
                } else {
                    personToItem.value(aktuellePerson)->setText(
                                aktuellePerson->getNachname() + ", " + aktuellePerson->getVorname());
                }
                ui->listWidget->sortItems();
            }
        }
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
                if (Einstellungen::getReihenfolgeVorNach()) {
                    personToItem.value(aktuellePerson)->setText(aktuellePerson->getName());
                } else {
                    personToItem.value(aktuellePerson)->setText(
                                aktuellePerson->getNachname() + ", " + aktuellePerson->getVorname());
                }
                ui->listWidget->sortItems();
            }
        }
    }
}

void PersonalWindow::on_checkAktiv_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAktiv(checked);
        refresh();
    }
}

void PersonalWindow::on_spinKm_valueChanged(int arg1)
{
    if (enabled) {
        aktuellePerson->setStrecke(arg1);
        ui->labelKilometerSum->setText(QString::number(aktuellePerson->getZeiten(Kilometer)));
    }
}

void PersonalWindow::on_checkTf_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungTf(checked);
        refresh();
    }
}
void PersonalWindow::on_checkZf_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungZf(checked);
        refresh();
    }
}
void PersonalWindow::on_checkRangierer_clicked(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungRangierer(checked);
        refresh();
    }
}

void PersonalWindow::on_dateDienst_dateChanged(const QDate &date)
{
    if (enabled) {
        aktuellePerson->setTauglichkeit(date);
    }
}
void PersonalWindow::on_checkDienst_clicked(bool checked)
{
    if (enabled) {
        ui->dateDienst->setEnabled(!checked);
        if (checked) {
            aktuellePerson->setTauglichkeit(QDate());
        } else {
            ui->dateDienst->setDate(QDate::currentDate());
            aktuellePerson->setTauglichkeit(QDate::currentDate());
        }
    }
}

void PersonalWindow::on_plainBemerkung_textChanged()
{
    if (enabled) {
        aktuellePerson->setBemerkungen(ui->plainBemerkung->toPlainText());
    }
}

void PersonalWindow::on_pushDelete_clicked()
{
    if (enabled) {
        if (aktuellePerson->getZeiten(Anzahl) > 0) {
            QMessageBox::information(this, tr("Warnung"), tr("Die ausgewählte Person kann nicht gelöscht werden, da Sie noch bei Aktivitäten eingetragen ist.\nBitte lösen Sie diese Verbindung bevor Sie die Person löschen!"));
            return;
        }
        if (QMessageBox::question(this, tr("Wirklich löschen"), tr("Möchten Sie die Person wirklich unwiderruflich löschen und aus dem System entfernen.\nFür ausgetretene Mitgleider können Sie auch ein Austrittsdatum angeben!")) != QMessageBox::Yes) {
            return;
        }
        enabled = false;

        QListWidgetItem *i = personToItem.value(aktuellePerson);
        ui->listWidget->takeItem(ui->listWidget->row(i));

        personToItem.remove(aktuellePerson);

        manager->removePerson(aktuellePerson);

        delete aktuellePerson;

        aktuellePerson = nullptr;
        toggleFields(false);
        refresh();
    }
}

void PersonalWindow::on_lineTf_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Tf, arg1);
}
void PersonalWindow::on_lineZf_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Zf, arg1);
}
void PersonalWindow::on_lineZub_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Zub, arg1);
}
void PersonalWindow::on_lineService_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Service, arg1);
}
void PersonalWindow::on_lineZugVorbereiten_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(ZugVorbereiten, arg1);
}
void PersonalWindow::on_lineWerkstatt_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Werkstatt, arg1);
}
void PersonalWindow::on_lineBuero_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Buero, arg1);
}
void PersonalWindow::on_lineAusbildung_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Ausbildung, arg1);
}
void PersonalWindow::on_lineInfrastruktur_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Infrastruktur, arg1);
}
void PersonalWindow::on_lineSonstiges_textChanged(const QString &arg1)
{
    setZeitenNachVeraenderung(Sonstiges, arg1);
}

void PersonalWindow::on_doubleAnzahl_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Anzahl, int(arg1));
        updateZeiten();
        refresh();
    }
}
void PersonalWindow::on_doubleKilometer_valueChanged(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Kilometer, int(arg1));
        updateZeiten();
        refresh();
    }
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
    ui->checkAktiv->setChecked(p->getAktiv());
    ui->spinKm->setValue(p->getStrecke());

    // Betriebsdienst
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());

    ui->checkDienst->setChecked(p->getTauglichkeit().isNull());
    ui->dateDienst->setEnabled(p->getTauglichkeit().isValid());
    ui->dateDienst->setDate(p->getTauglichkeit());

    // Sonstiges
    ui->plainBemerkung->setPlainText(p->getBemerkungen().replace("<br/>","\n"));

    // ** Aktivitaeten
    while(ui->tabelle->rowCount() > 0) ui->tabelle->removeRow(0);
    QMultiMap<AActivity*,Category> liste = p->getActivities();
    bool sortingSaved = ui->tabelle->isSortingEnabled();
    ui->tabelle->setSortingEnabled(false);
    for(AActivity *a: liste.uniqueKeys()) {
        for(Category cat: liste.values(a)) {
            // Datum
            ui->tabelle->insertRow(0);
            QTableWidgetItem *i0 = new QTableWidgetItem();
            i0->setData(Qt::EditRole, a->getDatum());
            ui->tabelle->setItem(0, 0, i0);

            Infos infos = a->getIndividual(p, cat);

            ui->tabelle->setItem(0, 1, new QTableWidgetItem(getLocalizedStringFromCategory(infos.kategorie)));
            QTime duration = QTime::fromMSecsSinceStartOfDay(infos.beginn.msecsTo(infos.ende));
            ui->tabelle->setItem(0, 2, new QTableWidgetItem(duration.toString("hh:mm")));
            ui->tabelle->setItem(0, 3, new QTableWidgetItem(a->getListStringShort()+(infos.bemerkung!= "" ? "<br/>"+infos.bemerkung : "")));
        }
    }
    ui->tabelle->setSortingEnabled(sortingSaved);
    ui->tabelle->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // ** Konto
    updateZeiten();

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

    ui->labelMinTf->setText(minutesToHourString(p->getMinimumStunden(Tf)));
    ui->labelMinZf->setText(minutesToHourString(p->getMinimumStunden(Zf)));
    ui->labelMinZub->setText(minutesToHourString(p->getMinimumStunden(Zub)));
    ui->labelMinService->setText(minutesToHourString(p->getMinimumStunden(Service)));
    ui->labelMinZugVorbereiten->setText(minutesToHourString(p->getMinimumStunden(ZugVorbereiten)));
    ui->labelMinWerkstatt->setText(minutesToHourString(p->getMinimumStunden(Werkstatt)));
    ui->labelMinBuero->setText(minutesToHourString(p->getMinimumStunden(Buero)));
    ui->labelMinAusbildung->setText(minutesToHourString(p->getMinimumStunden(Ausbildung)));
    ui->labelMinInfrastruktur->setText(minutesToHourString(p->getMinimumStunden(Infrastruktur)));
    ui->labelMinSonstiges->setText(minutesToHourString(p->getMinimumStunden(Sonstiges)));

    enabled = true;
}

QList<Person*> PersonalWindow::getSortierteListe()
{
    QList<Person*> liste = QList<Person*>();
    for(int i = 0; i < ui->tabelleGesamt->rowCount(); i++) {
        QString name = ui->tabelleGesamt->item(i, 0)->text();
        if (name != "") name += " ";
        name += ui->tabelleGesamt->item(i, 1)->text();
        Person *pers = manager->getPerson(name);
        if (pers != nullptr)
            liste.append(pers);
    }
    return liste;
}

void PersonalWindow::toggleFields(bool state)
{
    ui->lineID->setEnabled(state);
    ui->lineVorname->setEnabled(state);
    ui->lineNachname->setEnabled(state);
    ui->checkAktiv->setEnabled(state);

    ui->checkRangierer->setEnabled(state);
    ui->checkTf->setEnabled(state);
    ui->checkZf->setEnabled(state);
    ui->checkDienst->setEnabled(state);
    ui->dateDienst->setEnabled(false);

    ui->pushMailEinzel->setEnabled(state);

    ui->spinKm->setEnabled(state);
    ui->plainBemerkung->setEnabled(state);
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

    ui->pushEinzelPDF->setEnabled(state);
    ui->pushEinzelDrucken->setEnabled(state);
}

void PersonalWindow::setZeitenNachVeraenderung(Category cat, QString arg)
{
    if (enabled) {
        aktuellePerson->setAdditional(cat, minutesFromStringForDurationEditor(arg));
        updateZeiten();
        refresh();
    }
}

void PersonalWindow::updateZeiten()
{
    ui->labelTfSum->setText(minutesToHourString(aktuellePerson->getZeiten(Tf)));
    ui->labelTfSum->repaint();
    ui->labelZfSum->setText(minutesToHourString(aktuellePerson->getZeiten(Zf)));
    ui->labelZfSum->repaint();
    ui->labelZubSum->setText(minutesToHourString(aktuellePerson->getZeiten(Zub)));
    ui->labelZubSum->repaint();
    ui->labelServiceSum->setText(minutesToHourString(aktuellePerson->getZeiten(Service)));
    ui->labelServiceSum->repaint();
    ui->labelZugVorbereitenSum->setText(minutesToHourString(aktuellePerson->getZeiten(ZugVorbereiten)));
    ui->labelZugVorbereitenSum->repaint();
    ui->labelWerkstattSum->setText(minutesToHourString(aktuellePerson->getZeiten(Werkstatt)));
    ui->labelWerkstattSum->repaint();
    ui->labelBueroSum->setText(minutesToHourString(aktuellePerson->getZeiten(Buero)));
    ui->labelBueroSum->repaint();
    ui->labelAusbildungSum->setText(minutesToHourString(aktuellePerson->getZeiten(Ausbildung)));
    ui->labelAusbildungSum->repaint();
    ui->labelInfrastrukturSum->setText(minutesToHourString(aktuellePerson->getZeiten(Infrastruktur)));
    ui->labelInfrastrukturSum->repaint();
    ui->labelSonstigesSum->setText(minutesToHourString(aktuellePerson->getZeiten(Sonstiges)));
    ui->labelSonstigesSum->repaint();
    ui->labelAnzahlSum->setText(QString::number(aktuellePerson->getZeiten(Anzahl)));
    ui->labelAnzahlSum->repaint();
    ui->labelKilometerSum->setText(QString("%1 km").arg(aktuellePerson->getZeiten(Kilometer)));
    ui->labelKilometerSum->repaint();
    ui->labelGesamt->setText(QString("%1 (%2)")
                             .arg(minutesToHourString(aktuellePerson->getZeiten(Gesamt)))
                             .arg(minutesToHourString(aktuellePerson->getMinimumStunden(Gesamt))));
    ui->labelGesamt->repaint();
}

void PersonalWindow::on_pushMailEinzel_clicked()
{
    if (aktuellePerson->getMail() != "") {
        QDesktopServices::openUrl(QUrl("mailto:"+aktuellePerson->getMail()));
    }
}
