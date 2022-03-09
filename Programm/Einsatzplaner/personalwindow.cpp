#include "personalwindow.h"
#include "ui_personalwindow.h"
#include "person.h"
#include "export.h"
#include "minimumhourseditordialog.h"
#include "fileio.h"
#include "guihelper.h"

#include <QMessageBox>
#include <math.h>
#include <cmath>
#include <QDesktopServices>

PersonalWindow::PersonalWindow(CoreMainWindow *parent, ManagerPersonal *m) : QMainWindow(parent), ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);
    parentWindow = parent;
    connect(ui->comboAnzeige, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
    connect(ui->actionAktualisieren, SIGNAL(triggered()), this, SLOT(refresh()));

    connect(ui->actionPersonAdd, &QAction::triggered, this, &PersonalWindow::addPerson);
    connect(ui->actionMindeststunden, &QAction::triggered, this, &PersonalWindow::editMinimumhours);

    connect(ui->actionZeitenEinzelEinzelPDF, &QAction::triggered, this, &PersonalWindow::exportTimesDetailOnePdf);
    connect(ui->actionZeitenEinzelEinzelDrucken, &QAction::triggered, this, &PersonalWindow::exportTimesDetailOnePrint);

    connect(ui->actionZeitenEinzelListePDF, &QAction::triggered, this, &PersonalWindow::exportTimesDetailMultiplePdf);
    connect(ui->actionZeitenEinzelListeDrucken, &QAction::triggered, this, &PersonalWindow::exportTimesDetailMultiplePrint);

    connect(ui->actionZeitenListePDF, &QAction::triggered, this, &PersonalWindow::exportTimesListPdf);
    connect(ui->actionZeitenListeDrucken, &QAction::triggered, this, &PersonalWindow::exportTimesListPrint);


    connect(ui->actionMitgliederEinzelEinzelPDF, &QAction::triggered, this, &PersonalWindow::exportMemberDetailOnePdf);
    connect(ui->actionMitgliederEinzelEinzelDrucken, &QAction::triggered, this, &PersonalWindow::exportMemberDetailOnePrint);

    connect(ui->actionMitgliederEinzelListePDF, &QAction::triggered, this, &PersonalWindow::exportMemberDetailMultiplePdf);
    connect(ui->actionMitgliederEinzelListeDrucken, &QAction::triggered, this, &PersonalWindow::exportMemberDetailMultiplePrint);

    connect(ui->actionMitgliederListePDF, &QAction::triggered, this, &PersonalWindow::exportMemberListPdf);
    connect(ui->actionMitgliederListeDrucken, &QAction::triggered, this, &PersonalWindow::exportMemberListPrint);
    connect(ui->actionMitgliederListeCSV, &QAction::triggered, this, &PersonalWindow::exportMemberListCsv);

    connect(ui->actionBeitraegeRegulaerCSV, &QAction::triggered, this, &PersonalWindow::exportDuesRegularCsv);
    connect(ui->actionBeitraegeNachzahlungCSV, &QAction::triggered, this, &PersonalWindow::exportDuesAdditionalCsv);


    connect(ui->pushEmail, &QPushButton::clicked, this, &PersonalWindow::sendMailList);

    connect(ui->tabelleGesamt, &QTableWidget::cellDoubleClicked, this, &PersonalWindow::persShowFromTable);

    connect(ui->checkShowGesamt, &QCheckBox::clicked, this, &PersonalWindow::viewShowGesamt);
    connect(ui->checkShowAnzahl, &QCheckBox::clicked, this, &PersonalWindow::viewShowAnzahl);
    connect(ui->checkShowTf, &QCheckBox::clicked, this, &PersonalWindow::viewShowTf);
    connect(ui->checkShowTb, &QCheckBox::clicked, this, &PersonalWindow::viewShowTb);
    connect(ui->checkShowZf, &QCheckBox::clicked, this, &PersonalWindow::viewShowZf);
    connect(ui->checkShowZub, &QCheckBox::clicked, this, &PersonalWindow::viewShowZub);
    connect(ui->checkShowService, &QCheckBox::clicked, this, &PersonalWindow::viewShowService);
    connect(ui->checkShowVorbereiten, &QCheckBox::clicked, this, &PersonalWindow::viewShowVorbereiten);
    connect(ui->checkShowWerkstatt, &QCheckBox::clicked, this, &PersonalWindow::viewShowWerkstatt);
    connect(ui->checkShowBuero, &QCheckBox::clicked, this, &PersonalWindow::viewShowBuero);
    connect(ui->checkShowAusbildung, &QCheckBox::clicked, this, &PersonalWindow::viewShowAusbildung);
    connect(ui->checkShowInfrastruktur, &QCheckBox::clicked, this, &PersonalWindow::viewShowInfrastruktur);
    connect(ui->checkShowSonstiges, &QCheckBox::clicked, this, &PersonalWindow::viewShowSonstiges);
    connect(ui->checkShowKilometer, &QCheckBox::clicked, this, &PersonalWindow::viewShowKilometer);

    connect(ui->listWidget, &QListWidget::itemClicked, this, &PersonalWindow::persShowFromList);

    connect(ui->lineVorname, &QLineEdit::textChanged, this, &PersonalWindow::persSetVorname);
    connect(ui->lineNachname, &QLineEdit::textChanged, this, &PersonalWindow::persSetNachname);

    connect(ui->checkAktiv, &QCheckBox::clicked, this, &PersonalWindow::persSetAktiv);

    connect(ui->checkTf, &QCheckBox::clicked, this, &PersonalWindow::persSetTf);
    connect(ui->checkZf, &QCheckBox::clicked, this, &PersonalWindow::persSetZf);
    connect(ui->checkRangierer, &QCheckBox::clicked, this, &PersonalWindow::persSetRangierer);

    connect(ui->dateDienst, &QDateEdit::dateChanged, this, &PersonalWindow::persSetDienst);
    connect(ui->checkDienst, &QCheckBox::clicked, this, &PersonalWindow::persSetDienstUnkown);

    connect(ui->plainBemerkung, &QPlainTextEdit::textChanged, this, &PersonalWindow::persSetBemerkung);
    connect(ui->plainAusbildung, &QPlainTextEdit::textChanged, this, &PersonalWindow::persSetAusbildung);
    connect(ui->plainBetrieb, &QPlainTextEdit::textChanged, this, &PersonalWindow::persSetBetrieb);

    connect(ui->pushDelete, &QPushButton::clicked, this, &PersonalWindow::persDelete);

    connect(ui->pushPersonKomplett, &QPushButton::clicked, this, &PersonalWindow::persShowDetails);

    connect(ui->lineTf, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalTf);
    connect(ui->lineTb, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalTb);
    connect(ui->lineZf, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalZf);
    connect(ui->lineZub, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalZub);
    connect(ui->lineService, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalService);
    connect(ui->lineZugVorbereiten, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalVorbereiten);
    connect(ui->lineWerkstatt, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalWerkstatt);
    connect(ui->lineBuero, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalBuero);
    connect(ui->lineAusbildung, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalAusbildung);
    connect(ui->lineInfrastruktur, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalInfrastruktur);
    connect(ui->lineSonstiges, &QLineEdit::textChanged, this, &PersonalWindow::persSetAdditionalSonstiges);
    connect(ui->doubleAnzahl, &QDoubleSpinBox::valueChanged, this, &PersonalWindow::persSetAdditionalAnzahl);
    connect(ui->doubleKilometer, &QDoubleSpinBox::valueChanged, this, &PersonalWindow::persSetAdditionalKilometer);

    connect(ui->pushMailEinzel , &QPushButton::clicked, this, &PersonalWindow::sendMailCurrent);

    // Initalisieren der Statischen variablen
    manager = m;
    setWindowTitle(tr("Mitgliederverwaltung"));

    personToItem = QHash<Person*, QListWidgetItem*>();
    enabled = false;

    current = QList<Person*>();
    filter = Status::Aktiv;

    anzeige = QSet<Category>();
    ui->checkShowGesamt->setChecked(true);
    ui->checkShowAnzahl->setChecked(true);
    ui->checkShowKilometer->setChecked(true);
    viewShowGesamt(true);
    viewShowAnzahl(true);
    viewShowKilometer(true);
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
    refreshTabelle();
    refreshEinzel();
}
void PersonalWindow::refreshTabelle()
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
        ui->tabelleGesamt->setHorizontalHeaderItem(2, new QTableWidgetItem(toString(curr)));
        if (origSortName == toString(curr)) newSort = 2;
        else newSort++;
    }
    ui->tabelleGesamt->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Einzelne Personen einfuegen und Summe berechnen
    int pos;
    QMap<Category, int> sum;
    for (Person *p: qAsConst(current)) {
        p->berechne();
        QString farbe = Person::FARBE_STANDARD;
        switch (p->pruefeStunden()) {
        case AktivOhne:  farbe = Person::FARBE_FEHLENDE_STUNDEN; break;
        case PassivMit: farbe = Person::FARBE_GENUG_STUNDEN; break;
        default: farbe = Person::FARBE_STANDARD;
        }
        ui->tabelleGesamt->insertRow(0);
        PersonTableWidgetItem *i = new PersonTableWidgetItem(p, p->getVorname());
        faerbeZelle(i, farbe);
        ui->tabelleGesamt->setItem(0, 0, i);

        i = new PersonTableWidgetItem(p, p->getNachname());
        faerbeZelle(i, farbe);
        ui->tabelleGesamt->setItem(0, 1, i);

        pos = 2;
        for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
            if (! anzeige.contains(cat)) continue;
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
            i = new PersonTableWidgetItem(p);
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
            case AktivOhne:
                faerbeZelle(i, Person::FARBE_FEHLENDE_STUNDEN); break;
            case PassivMit:
                faerbeZelle(i, Person::FARBE_GENUG_STUNDEN); break;
            default:
                faerbeZelle(i, Person::FARBE_STANDARD); break;
            }
            ui->tabelleGesamt->setItem(0, pos++, i);
        }
    }

    // Zeile Gesamt einfuegen
    ui->tabelleGesamt->insertRow(0);
    ui->tabelleGesamt->setItem(0, 0, new PersonTableWidgetItem(nullptr));
    ui->tabelleGesamt->setItem(0, 1, new PersonTableWidgetItem(nullptr, tr(" Gesamt")));
    pos = 2;
    QTableWidgetItem *ii;
    for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        if (! anzeige.contains(cat)) continue;
        ii = new PersonTableWidgetItem(nullptr);
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
    for (Person *p: qAsConst(current)) {
        QListWidgetItem *item = new PersonListWidgetItem(p, p->getNameSortierung());
        switch (p->pruefeStunden()) {
        case AktivOhne:
            item->setBackground(QBrush(QColor(Person::FARBE_FEHLENDE_STUNDEN)));
            item->setForeground(QBrush(QColor("black")));
            break;
        case PassivMit:
            item->setBackground(QBrush(QColor(Person::FARBE_GENUG_STUNDEN)));
            item->setForeground(QBrush(QColor("black")));
            break;
        default:
            break;
        }
        ui->listWidget->insertItem(0, item);
        personToItem.insert(p, item);
    }
    ui->listWidget->sortItems();
}

void PersonalWindow::editMinimumhours()
{
   if (MinimumHoursEditorDialog(manager, this).exec() == QDialog::Accepted) {
       refresh();
   }
}

void PersonalWindow::exportTimesDetailOnePdf()
{
    aktuellePerson->printZeiten(Export::getPrinterPDF(this, "Zeiten-"+aktuellePerson->getName(), QPageLayout::Orientation::Portrait));
}
void PersonalWindow::exportTimesDetailOnePrint()
{
    aktuellePerson->printZeiten(Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}

void PersonalWindow::exportTimesDetailMultiplePdf()
{
    manager->printZeitenEinzel(getSortierteListe(), filter,
                        Export::getPrinterPDF(this, "Zeiten", QPageLayout::Orientation::Portrait));
}
void PersonalWindow::exportTimesDetailMultiplePrint()
{
    manager->printZeitenEinzel(getSortierteListe(), filter,
                        Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}

void PersonalWindow::exportTimesListPdf()
{
    manager->printZeitenListe(
                getSortierteListe(), anzeige, filter,
                Export::getPrinterPDF(this, "Einsatzzeiten", QPageLayout::Orientation::Landscape));
}
void PersonalWindow::exportTimesListPrint()
{
    manager->printZeitenListe(
                getSortierteListe(), anzeige, filter,
                Export::getPrinterPaper(this, QPageLayout::Orientation::Landscape));
}

void PersonalWindow::exportMemberDetailOnePdf()
{
    aktuellePerson->printPersonaldaten(Export::getPrinterPDF(this, "Stammdaten-"+aktuellePerson->getName(), QPageLayout::Orientation::Portrait));
}
void PersonalWindow::exportMemberDetailOnePrint()
{
    aktuellePerson->printPersonaldaten(Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}

void PersonalWindow::exportMemberDetailMultiplePdf()
{
    manager->printMitgliederEinzel(getSortierteListe(), filter,
                        Export::getPrinterPDF(this, "Stammdaten", QPageLayout::Orientation::Portrait));
}
void PersonalWindow::exportMemberDetailMultiplePrint()
{
    manager->printMitgliederEinzel(getSortierteListe(), filter,
                        Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}

void PersonalWindow::exportMemberListPdf()
{
    manager->printMitgliederListe(getSortierteListe(), filter, QSet<QString>(),
                            Export::getPrinterPDF(this, "Mitgliederliste", QPageLayout::Orientation::Portrait));
}
void PersonalWindow::exportMemberListPrint()
{
    manager->printMitgliederListe(getSortierteListe(), filter, QSet<QString>(),
                            Export::getPrinterPaper(this, QPageLayout::Orientation::Landscape));
}
void PersonalWindow::exportMemberListCsv()
{
    manager->saveMitgliederListeAlsCSV(current,
                                  FileIO::getFilePathSave(this, "Mitgliederliste", FileIO::DateiTyp::CSV));
}

void PersonalWindow::exportDuesRegularCsv()
{
    manager->saveBeitraegeRegulaerAlsCSV(FileIO::getFilePathSave(this, "Beitraege-Regulaer", FileIO::DateiTyp::CSV));
}
void PersonalWindow::exportDuesAdditionalCsv()
{
    manager->saveBeitraegeNachzahlungAlsCSV(FileIO::getFilePathSave(this, "Beitraege-Nachzahlung", FileIO::DateiTyp::CSV));
}

void PersonalWindow::sendMailList()
{
    if (current.isEmpty()) return;
    QSet<QString> mails;
    QList<Person*> keineMail;
    for (Person *p: qAsConst(current)) {
        if (p->getMail() != "") {
            mails.insert(p->getMail());
        } else {
            keineMail.append(p);
        }
    }
    if (! mails.isEmpty()) {
        QString betreff = "&subject=[%1]";
        if (filter == Status::AlleMitglieder) {
            betreff = betreff.arg(tr("AkO-Alle"));
        } else if (filter == Status::Aktiv) {
            betreff = betreff.arg(tr("AkO-Aktive"));
        } else if (filter == Status::Passiv) {
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

void PersonalWindow::persShowFromTable(int row, int column)
{
    PersonTableWidgetItem *ptwi = static_cast<PersonTableWidgetItem*>(ui->tabelleGesamt->item(row, column));
    if (ptwi->getPerson() == nullptr)
        return;

    showPerson(ptwi->getPerson());
    ui->tabWidgetMain->setCurrentIndex(1);
}

void PersonalWindow::viewShowGesamt(bool checked)
{
    if (checked) anzeige.insert(Category::Gesamt);
    else anzeige.remove(Category::Gesamt);
    refreshTabelle();
}
void PersonalWindow::viewShowAnzahl(bool checked)
{
    if (checked) anzeige.insert(Category::Anzahl);
    else anzeige.remove(Category::Anzahl);
    refreshTabelle();
}
void PersonalWindow::viewShowTf(bool checked)
{
    if (checked) anzeige.insert(Category::Tf);
    else anzeige.remove(Category::Tf);
    refreshTabelle();
}
void PersonalWindow::viewShowTb(bool checked)
{
    if (checked) anzeige.insert(Category::Tb);
    else anzeige.remove(Category::Tb);
    refreshTabelle();
}
void PersonalWindow::viewShowZf(bool checked)
{
    if (checked) anzeige.insert(Category::Zf);
    else anzeige.remove(Category::Zf);
    refreshTabelle();
}
void PersonalWindow::viewShowZub(bool checked)
{
    if (checked) anzeige.insert(Category::Zub);
    else anzeige.remove(Category::Zub);
    refreshTabelle();
}
void PersonalWindow::viewShowService(bool checked)
{
    if (checked) anzeige.insert(Category::Service);
    else anzeige.remove(Category::Service);
    refreshTabelle();
}
void PersonalWindow::viewShowVorbereiten(bool checked)
{
    if (checked) anzeige.insert(Category::ZugVorbereiten);
    else anzeige.remove(Category::ZugVorbereiten);
    refreshTabelle();
}
void PersonalWindow::viewShowWerkstatt(bool checked)
{
    if (checked) anzeige.insert(Category::Werkstatt);
    else anzeige.remove(Category::Werkstatt);
    refreshTabelle();
}
void PersonalWindow::viewShowBuero(bool checked)
{
    if (checked) anzeige.insert(Category::Buero);
    else anzeige.remove(Category::Buero);
    refreshTabelle();
}
void PersonalWindow::viewShowAusbildung(bool checked)
{
    if (checked) anzeige.insert(Category::Ausbildung);
    else anzeige.remove(Category::Ausbildung);
    refreshTabelle();
}
void PersonalWindow::viewShowInfrastruktur(bool checked)
{
    if (checked) anzeige.insert(Category::Infrastruktur);
    else anzeige.remove(Category::Infrastruktur);
    refreshTabelle();
}
void PersonalWindow::viewShowSonstiges(bool checked)
{
    if (checked) anzeige.insert(Category::Sonstiges);
    else anzeige.remove(Category::Sonstiges);
    refreshTabelle();
}
void PersonalWindow::viewShowKilometer(bool checked)
{
    if (checked) anzeige.insert(Category::Kilometer);
    else anzeige.remove(Category::Kilometer);
    refreshTabelle();
}

void PersonalWindow::addPerson()
{
    Person *p = manager->newPerson();
    if (p == nullptr)
        return;

    refresh();
    showPerson(p);
}

void PersonalWindow::persShowFromList(QListWidgetItem *item)
{
    showPerson(static_cast<PersonListWidgetItem*>(item)->getPerson());
}

void PersonalWindow::persSetVorname(const QString &arg1)
{
    if (enabled) {
        if (aktuellePerson->setVorname(arg1)) {
            if (personToItem.contains(aktuellePerson)) {
                personToItem.value(aktuellePerson)->setText(aktuellePerson->getNameSortierung());
                ui->listWidget->sortItems();
            }
        } else {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        }
    }
}
void PersonalWindow::persSetNachname(const QString &arg1)
{
    if (enabled) {
        if (aktuellePerson->setNachname(arg1)) {
            if (personToItem.contains(aktuellePerson)) {
                personToItem.value(aktuellePerson)->setText(aktuellePerson->getNameSortierung());
                ui->listWidget->sortItems();
            }
        } else {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        }
    }
}

void PersonalWindow::persSetAktiv(bool checked)
{
    if (enabled) {
        aktuellePerson->setAktiv(checked);
        refresh();
    }
}

void PersonalWindow::persSetTf(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungTf(checked);
        refresh();
    }
}
void PersonalWindow::persSetZf(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungZf(checked);
        refresh();
    }
}
void PersonalWindow::persSetRangierer(bool checked)
{
    if (enabled) {
        aktuellePerson->setAusbildungRangierer(checked);
        refresh();
    }
}

void PersonalWindow::persSetDienst(const QDate &date)
{
    if (enabled) {
        aktuellePerson->setTauglichkeit(date);
        refresh();
    }
}
void PersonalWindow::persSetDienstUnkown(bool checked)
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

void PersonalWindow::persSetBemerkung()
{
    if (enabled) {
        aktuellePerson->setBemerkungen(ui->plainBemerkung->toPlainText().replace("\n","<br/>"));
    }
}
void PersonalWindow::persSetAusbildung()
{
    if (enabled)
        aktuellePerson->setSonstigeAusbildung(ui->plainAusbildung->toPlainText().replace("\n","<br/>"));
}
void PersonalWindow::persSetBetrieb()
{
    if (enabled)
        aktuellePerson->setSonstigeBetrieblich(ui->plainBetrieb->toPlainText().replace("\n","<br/>"));
}

void PersonalWindow::persDelete()
{
    if (enabled) {
        if (!parentWindow->loeschenPerson(aktuellePerson)) {
            return;
        }
        enabled = false;

        ui->listWidget->takeItem(ui->listWidget->row(personToItem.value(aktuellePerson)));
        personToItem.remove(aktuellePerson);

        delete aktuellePerson;

        toggleFields(false);
        refresh();
    }
}

void PersonalWindow::persShowDetails()
{
    parentWindow->openPerson(aktuellePerson);
}

void PersonalWindow::persSetAdditionalTf(const QString &arg1)
{
    setZeitenNachVeraenderung(Tf, arg1);
}
void PersonalWindow::persSetAdditionalTb(const QString &arg1)
{
    setZeitenNachVeraenderung(Tb, arg1);
}
void PersonalWindow::persSetAdditionalZf(const QString &arg1)
{
    setZeitenNachVeraenderung(Zf, arg1);
}
void PersonalWindow::persSetAdditionalZub(const QString &arg1)
{
    setZeitenNachVeraenderung(Zub, arg1);
}
void PersonalWindow::persSetAdditionalService(const QString &arg1)
{
    setZeitenNachVeraenderung(Service, arg1);
}
void PersonalWindow::persSetAdditionalVorbereiten(const QString &arg1)
{
    setZeitenNachVeraenderung(ZugVorbereiten, arg1);
}
void PersonalWindow::persSetAdditionalWerkstatt(const QString &arg1)
{
    setZeitenNachVeraenderung(Werkstatt, arg1);
}
void PersonalWindow::persSetAdditionalBuero(const QString &arg1)
{
    setZeitenNachVeraenderung(Buero, arg1);
}
void PersonalWindow::persSetAdditionalAusbildung(const QString &arg1)
{
    setZeitenNachVeraenderung(Ausbildung, arg1);
}
void PersonalWindow::persSetAdditionalInfrastruktur(const QString &arg1)
{
    setZeitenNachVeraenderung(Infrastruktur, arg1);
}
void PersonalWindow::persSetAdditionalSonstiges(const QString &arg1)
{
    setZeitenNachVeraenderung(Sonstiges, arg1);
}

void PersonalWindow::persSetAdditionalAnzahl(double arg1)
{
    if (enabled) {
        aktuellePerson->setAdditional(Anzahl, int(arg1));
        updateZeiten();
        refresh();
    }
}
void PersonalWindow::persSetAdditionalKilometer(double arg1)
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
    ui->lineVorname->setText(p->getVorname());
    ui->lineNachname->setText(p->getNachname());

    // ** Stammdaten
    // Allgemein
    ui->checkAktiv->setChecked(p->getAktiv());

    // Betriebsdienst
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());

    ui->checkDienst->setChecked(p->getTauglichkeit().isNull());
    ui->dateDienst->setEnabled(p->getTauglichkeit().isValid());
    ui->dateDienst->setDate(p->getTauglichkeit());

    // Sonstiges
    ui->plainBemerkung->setPlainText(p->getBemerkungen().replace("<br/>","\n"));
    ui->plainAusbildung->setPlainText(p->getSonstigeAusbildung().replace("<br/>","\n"));
    ui->plainBetrieb->setPlainText(p->getSonstigeBetrieblich().replace("<br/>","\n"));

    // ** Aktivitaeten
    while(ui->tabelle->rowCount() > 0) ui->tabelle->removeRow(0);
    const QList<Einsatz*> liste = p->getActivities();
    bool sortingSaved = ui->tabelle->isSortingEnabled();
    ui->tabelle->setSortingEnabled(false);
    for(Einsatz* e: liste) {
            // Datum
            ui->tabelle->insertRow(0);
            QTableWidgetItem *i0 = new QTableWidgetItem();
            i0->setData(Qt::EditRole, e->getActivity()->getDatum());
            ui->tabelle->setItem(0, 0, i0);

            ui->tabelle->setItem(0, 1, new QTableWidgetItem(toString(e->getKategorie())));
            ui->tabelle->setItem(0, 2, new QTableWidgetItem(QTime::fromMSecsSinceStartOfDay(e->getDauer() * 60000).toString("HH:mm")));
            QTableWidgetItem *i3 = new QTableWidgetItem(e->getActivity()->getStringShort()+(e->getBemerkung() != "" ? "\n"+e->getBemerkung() : ""));
            i3->setToolTip(e->getActivity()->getBemerkungen());
            ui->tabelle->setItem(0, 3, i3);
    }
    ui->tabelle->setSortingEnabled(sortingSaved);
    ui->tabelle->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // ** Konto
    updateZeiten();

    ui->lineTf->setText(stringForDurationEditorFromMinutes(p->getAdditional(Tf)));
    ui->lineTb->setText(stringForDurationEditorFromMinutes(p->getAdditional(Tb)));
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
    ui->labelMinTb->setText(minutesToHourString(p->getMinimumStunden(Tb)));
    ui->labelMinZf->setText(minutesToHourString(p->getMinimumStunden(Zf)));
    ui->labelMinZub->setText(minutesToHourString(p->getMinimumStunden(Zub)));
    ui->labelMinService->setText(minutesToHourString(p->getMinimumStunden(Service)));
    ui->labelMinZugVorbereiten->setText(minutesToHourString(p->getMinimumStunden(ZugVorbereiten)));
    ui->labelMinWerkstatt->setText(minutesToHourString(p->getMinimumStunden(Werkstatt)));
    ui->labelMinBuero->setText(minutesToHourString(p->getMinimumStunden(Buero)));
    ui->labelMinAusbildung->setText(minutesToHourString(p->getMinimumStunden(Ausbildung)));
    ui->labelMinInfrastruktur->setText(minutesToHourString(p->getMinimumStunden(Infrastruktur)));
    ui->labelMinSonstiges->setText(minutesToHourString(p->getMinimumStunden(Sonstiges)));
    ui->labelMinGesamt->setText(minutesToHourString(p->getMinimumStunden(Gesamt)));

    enabled = true;
}

QList<Person*> PersonalWindow::getSortierteListe()
{
    QList<Person*> liste = QList<Person*>();
    for(int i = 0; i < ui->tabelleGesamt->rowCount(); i++) {
        PersonTableWidgetItem *ptwi = static_cast<PersonTableWidgetItem*>(ui->tabelleGesamt->item(i, 0));
        if (ptwi->getPerson() != nullptr)
            liste.append(ptwi->getPerson());
    }
    return liste;
}

void PersonalWindow::toggleFields(bool state)
{
    ui->lineVorname->setEnabled(state);
    ui->lineNachname->setEnabled(state);
    ui->checkAktiv->setEnabled(state);

    ui->checkRangierer->setEnabled(state);
    ui->checkTf->setEnabled(state);
    ui->checkZf->setEnabled(state);
    ui->checkDienst->setEnabled(state);
    ui->dateDienst->setEnabled(false);

    ui->pushMailEinzel->setEnabled(state);

    ui->plainBemerkung->setEnabled(state);
    ui->plainAusbildung->setEnabled(state);
    ui->plainBetrieb->setEnabled(state);
    ui->pushDelete->setEnabled(state);

    ui->tabelle->setEnabled(state);

    ui->lineTf->setEnabled(state);
    ui->lineTb->setEnabled(state);
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

    ui->pushEinzelDrucken->setEnabled(state);

    ui->pushPersonKomplett->setEnabled(state);
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
    ui->labelTbSum->setText(minutesToHourString(aktuellePerson->getZeiten(Tb)));
    ui->labelTbSum->repaint();
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
    ui->labelGesamtSum->setText(minutesToHourString(aktuellePerson->getZeiten(Gesamt)));
    ui->labelGesamtSum->repaint();
}

void PersonalWindow::faerbeZelle(QTableWidgetItem *item, QString hintergrund, QString vordergrund)
{
    if (hintergrund != Person::FARBE_STANDARD) {
        item->setBackground(QBrush(QColor(hintergrund)));
        item->setForeground(QBrush(QColor(vordergrund)));
    }
}

void PersonalWindow::sendMailCurrent()
{
    if (aktuellePerson->getMail() != "") {
        QDesktopServices::openUrl(QUrl("mailto:"+aktuellePerson->getMail()));
    }
}
