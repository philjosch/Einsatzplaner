#include "mainwindowpersonal.h"
#include "ui_mainwindowpersonal.h"

#include "fileio.h"
#include "minimumhourseditordialog.h"
#include "export.h"
#include "personwindow.h"
#include "guihelper.h"
#include "beitraegeeditordialog.h"

#include <QDesktopServices>
#include <QMessageBox>

MainWindowPersonal::MainWindowPersonal(EplFile *file) :
    CoreMainWindow(file), ui(new Ui::MainWindowPersonal)
{
    ui->setupUi(this);

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindowPersonal::showPreferences);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindowPersonal::showAboutQt);
    connect(ui->actionAboutApp, &QAction::triggered, this, &MainWindowPersonal::showAboutApp);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindowPersonal::closeApp);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindowPersonal::fileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindowPersonal::fileOpen);

    connect(ui->menuRecentlyused, &QMenu::aboutToShow, this, &MainWindowPersonal::updateRecentlyused);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindowPersonal::clearRecentlyUsed);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindowPersonal::fileSave);
    connect(ui->actionSaveas, &QAction::triggered, this, &MainWindowPersonal::fileSaveAs);
    connect(ui->actionSavePersonal, &QAction::triggered, this, &MainWindowPersonal::fileSavePersonal);

    connect(ui->actionSettings, &QAction::triggered, this, &MainWindowPersonal::showFileSettings);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindowPersonal::fileClose);


    connect(ui->actionAddPerson, &QAction::triggered, this, &MainWindowPersonal::addPerson);
    connect(ui->actionAktualisieren, &QAction::triggered, this, &MainWindowPersonal::refresh);
    connect(ui->actionMindeststunden, &QAction::triggered, this, &MainWindowPersonal::editMinimumhours);
    connect(ui->actionMailListe, &QAction::triggered, this, &MainWindowPersonal::sendMailList);
    connect(ui->actionMitgliedsbeitraege, &QAction::triggered, this, &MainWindowPersonal::editDues);

    connect(ui->actionMitgliederEinzelListePDF, &QAction::triggered, this, &MainWindowPersonal::exportMemberDetailMultiplePdf);
    connect(ui->actionMitgliederEinzelListeDrucken, &QAction::triggered, this, &MainWindowPersonal::exportMemberDetailMultiplePrint);

    connect(ui->actionMitgliederListePDF, &QAction::triggered, this, &MainWindowPersonal::exportMemberListPdf);
    connect(ui->actionMitgliederListeDrucken, &QAction::triggered, this, &MainWindowPersonal::exportMemberListPrint);
    connect(ui->actionMitgliederListeCSV, &QAction::triggered, this, &MainWindowPersonal::exportMemberListCsv);

    connect(ui->actionBeitraegeRegulaerCSV, &QAction::triggered, this, &MainWindowPersonal::exportDuesRegularCsv);
    connect(ui->actionBeitraegeNachzahlungCSV, &QAction::triggered, this, &MainWindowPersonal::exportDuesAdditionalCsv);

    connect(ui->comboAnzeige, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindowPersonal::filterChanged);
    connect(ui->tabelleMitglieder, &QTableWidget::cellDoubleClicked, this, &MainWindowPersonal::showPersFromTable);

    connect(ui->listAnzeige, &QListWidget::itemChanged, this, &MainWindowPersonal::viewShowColumFromItem);

    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    current = QList<Person*>();
    filter = Status::AlleMitglieder;
    anzeige = QSet<QString>();


    for(int i = 0; i < ui->listAnzeige->count(); ++i) {
        if (ui->listAnzeige->item(i)->checkState() == Qt::CheckState::Checked)
            viewShowColumFromItem(ui->listAnzeige->item(i));
    }

    updateWindowHeaders();
    refresh();
}
MainWindowPersonal::~MainWindowPersonal()
{
    delete ui;
}

bool MainWindowPersonal::open(QString path)
{
    EplFile* datei = getDateiVonPfad(path);
    if (datei == nullptr) return false;

    CoreMainWindow *mw = new MainWindowPersonal(datei);
    mw->show();
    return true;
}


CoreMainWindow *MainWindowPersonal::handlerNew()
{
    return new MainWindowPersonal();
}
void MainWindowPersonal::handlerOpen(QString path)
{
    open(path);
}


void MainWindowPersonal::addPerson()
{
    Person *neu = personal->newPerson();
    openPerson(neu);
    refresh();
}
void MainWindowPersonal::refresh()
{
    current = personal->getPersonen(filter);


    // Tabelle leeren
    ui->tabelleMitglieder->setRowCount(0);
    ui->tabelleMitglieder->setSortingEnabled(false);

    // Setup columns
    int clmn = 0;
    ui->tabelleMitglieder->setColumnCount(0);

    foreach (QString s, Person::ANZEIGE_PERSONALDATEN) {
        if (anzeige.contains(s)) {
            ui->tabelleMitglieder->insertColumn(clmn);
            ui->tabelleMitglieder->setHorizontalHeaderItem(clmn++, new QTableWidgetItem(s));
        }
    }

    QTableWidgetItem *i;
    for (Person *p: qAsConst(current)) {
        clmn = 0;
        ui->tabelleMitglieder->insertRow(0);

        if (anzeige.contains("Vorname"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getVorname()));
        if (anzeige.contains("Nachname"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getNachname()));
        if (anzeige.contains("Geburtsdatum")) {
            i = new PersonTableWidgetItem(p);
            i->setData(0, p->getGeburtstag());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Geschlecht"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, Person::toString(p->getGeschlecht())));
        if (anzeige.contains("Anrede"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getAnrede()));
        if (anzeige.contains("Beruf"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getBeruf()));


        if (anzeige.contains("Nummer")) {
            i = new PersonTableWidgetItem(p);
            i->setData(0, p->getNummer());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Eintritt")) {
            i = new PersonTableWidgetItem(p);
            i->setData(0, p->getEintritt());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Status")) {
            i = new PersonTableWidgetItem(p,
                        QString("%1%2")
                        .arg((p->isAusgetreten() ? "Ehemals ": ""),
                             (p->getAktiv() ? "Aktiv" : "Passiv")));
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Austritt")) {
            if (p->isAusgetreten() || p->getAustritt().isValid()) {
                i = new PersonTableWidgetItem(p);
                i->setData(0, p->getAustritt());
                ui->tabelleMitglieder->setItem(0, clmn++, i);
            } else {
                clmn++;
            }
        }
        if (anzeige.contains("Beitragsart"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, Person::toString(p->getBeitragsart())));
        if (anzeige.contains("IBAN"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getIban()));
        if (anzeige.contains("Bank"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getBank()));
        if (anzeige.contains("Kontoinhaber"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getKontoinhaber()));
        if (anzeige.contains("Beitrag")) {
            i = new PersonTableWidgetItem(p);
            if (p->getBeitrag() != 0)
                i->setData(0, p->getBeitrag()/100.f);
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Beitrag (Nachzahlung)")) {
            i = new PersonTableWidgetItem(p);
            if (p->getBeitragNachzahlung() != 0)
                i->setData(0, p->getBeitragNachzahlung()/100.f);
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }


        if (anzeige.contains("Straße"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getStrasse()));
        if (anzeige.contains("PLZ")) {
            i = new PersonTableWidgetItem(p);
            i->setData(0, p->getPLZ());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Ort"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getOrt()));
        if (anzeige.contains("Strecke")) {
            i = new PersonTableWidgetItem(p);
            i->setData(0, p->getStrecke());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }


        if (anzeige.contains("Mail"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getMail()));
        if (anzeige.contains("Telefon"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getTelefon()));
        if (anzeige.contains("Telefon2"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getTelefon2()));


        if (anzeige.contains("Tf"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getAusbildungTf() ? "Ja" : ""));
        if (anzeige.contains("Zf"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getAusbildungZf() ? "Ja" : ""));
        if (anzeige.contains("Rangierer"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getAusbildungRangierer() ? "Ja" : ""));
        if (anzeige.contains("Tauglichkeit")) {
            i = new PersonTableWidgetItem(p);
            i->setData(0, p->getTauglichkeit());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        }
        if (anzeige.contains("Bemerkung Betrieb."))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getSonstigeBetrieblich().replace("<br/>","\n")));
        if (anzeige.contains("Sonst. Ausbildung"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getSonstigeAusbildung().replace("<br/>","\n")));


        if (anzeige.contains("Mail Zustimmung"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getMailOK() ? "Ja": "Nein"));
        if (anzeige.contains("Telefon Zustimmung"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getTelefonOK() ? "Ja" : "Nein"));


        if (anzeige.contains("Bemerkung"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getBemerkungen().replace("<br/>","\n")));
    }

    ui->tabelleMitglieder->setSortingEnabled(true);
    ui->tabelleMitglieder->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    ui->labelMitgliederAnzMitglieder->setText(QString::number(personal->getAnzahlMitglieder(Status::AlleMitglieder)));
    ui->labelMitgliederAnzAktiv->setText(QString::number(personal->getAnzahlMitglieder(Status::Aktiv)));
    ui->labelMitgliederAnzPassiv->setText(QString::number(personal->getAnzahlMitglieder(Status::Passiv)));
    ui->labelMitgliederAnzAusgetreten->setText(QString::number(personal->getAnzahlMitglieder(Status::Ausgetreten)));
    ui->labelMitgliederAnzErfasst->setText(QString::number(personal->getAnzahlMitglieder(Status::Registriert)));
}
void MainWindowPersonal::editMinimumhours()
{
   MinimumHoursEditorDialog(personal, this).exec();
}
void MainWindowPersonal::sendMailList()
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
void MainWindowPersonal::editDues()
{
    BeitraegeEditorDialog(this, personal).exec();
}

void MainWindowPersonal::exportMemberDetailMultiplePdf()
{
    personal->printMitgliederEinzel(getSortierteListe(), filter,
                        Export::getPrinterPDF(this, "Stammdaten", QPageLayout::Orientation::Portrait));
}
void MainWindowPersonal::exportMemberDetailMultiplePrint()
{
    personal->printMitgliederEinzel(getSortierteListe(), filter,
                        Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}

void MainWindowPersonal::exportMemberListPdf()
{
    personal->printMitgliederListe(getSortierteListe(), filter, anzeige,
                            Export::getPrinterPDF(this, "Mitgliederliste", QPageLayout::Orientation::Portrait));
}
void MainWindowPersonal::exportMemberListPrint()
{
    personal->printMitgliederListe(getSortierteListe(), filter, anzeige,
                            Export::getPrinterPaper(this, QPageLayout::Orientation::Landscape));
}
void MainWindowPersonal::exportMemberListCsv()
{
    personal->saveMitgliederListeAlsCSV(current,
                                   FileIO::getFilePathSave(this, "Mitgliederliste", FileIO::DateiTyp::CSV));
}

void MainWindowPersonal::exportDuesRegularCsv()
{
    personal->saveBeitraegeRegulaerAlsCSV(FileIO::getFilePathSave(this, "Beitraege-Regulaer", FileIO::DateiTyp::CSV));
}
void MainWindowPersonal::exportDuesAdditionalCsv()
{
    personal->saveBeitraegeNachzahlungAlsCSV(FileIO::getFilePathSave(this, "Beitraege-Nachzahlung", FileIO::DateiTyp::CSV));
}

void MainWindowPersonal::filterChanged(int index)
{
    switch (index) {
    case 0: filter = AlleMitglieder; break;
    case 1: filter = Aktiv; break;
    case 2: filter = Passiv; break;
    case 3: filter = AktivMit; break;
    case 4: filter = AktivOhne; break;
    case 5: filter = PassivMit; break;
    case 6: filter = Ausgetreten; break;
    default: filter = Registriert; break;
    }
    refresh();
}

void MainWindowPersonal::showPersFromTable(int row, [[maybe_unused]] int column)
{
    PersonTableWidgetItem *clicked = dynamic_cast<PersonTableWidgetItem*>(ui->tabelleMitglieder->item(row, column));
    if (clicked != nullptr) {
        openPerson(clicked->getPerson());
    }
}


QList<Person*> MainWindowPersonal::getSortierteListe()
{
    QList<Person*> liste = QList<Person*>();
    for(int i = 0; i < ui->tabelleMitglieder->rowCount(); i++) {
        PersonTableWidgetItem *ptwi = static_cast<PersonTableWidgetItem*>(ui->tabelleMitglieder->item(i, 0));
        if (ptwi->getPerson() != nullptr)
            liste.append(ptwi->getPerson());
    }
    return liste;
}

void MainWindowPersonal::viewShowColumFromItem(QListWidgetItem *item)
{
    if (item->checkState() == Qt::CheckState::Checked) {
        anzeige.insert(item->text());
    } else {
        anzeige.remove(item->text());
    }
    refresh();
}
