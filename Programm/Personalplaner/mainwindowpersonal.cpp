#include "mainwindowpersonal.h"
#include "ui_mainwindowpersonal.h"

#include "fileio.h"
#include "minimumhourseditordialog.h"
#include "export.h"
#include "personwindow.h"
#include "guihelper.h"

#include <QDesktopServices>
#include <QMessageBox>

MainWindowPersonal::MainWindowPersonal(EplFile *file) :
    CoreMainWindow(file), ui(new Ui::MainWindowPersonal)
{
    ui->setupUi(this);

    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    current = QList<Person*>();
    filter = Status::AlleMitglieder;
    anzeige = QSet<QString>();

    fenster = QMap<Person*, PersonWindow*>();

    for(int i = 0; i < ui->listAnzeige->count(); ++i) {
        if (ui->listAnzeige->item(i)->checkState() == Qt::CheckState::Checked)
            on_listAnzeige_itemChanged(ui->listAnzeige->item(i));
    }

    updateWindowHeaders();
    on_actionAktualisieren_triggered();
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

void MainWindowPersonal::onDateiWurdeVeraendert()
{
    CoreMainWindow::onDateiWurdeVeraendert();
//    on_actionAktualisieren_triggered();
}

void MainWindowPersonal::onPersonWirdEntferntWerden(Person *p)
{
    if (fenster.contains(p)) {
        PersonWindow *w = fenster.value(p);
        fenster.remove(p);
        w->close();
        delete w;
    }
}
void MainWindowPersonal::onPersonWurdeBearbeitet([[maybe_unused]] Person *p)
{
    on_actionAktualisieren_triggered();
}


void MainWindowPersonal::on_actionAddPerson_triggered()
{
    Person *neu = personal->newPerson();
    showPerson(neu);
    on_actionAktualisieren_triggered();
}
void MainWindowPersonal::on_actionAktualisieren_triggered()
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
    for (Person *p: current) {
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
void MainWindowPersonal::on_actionMindeststunden_triggered()
{
   MinimumHoursEditorDialog(personal, this).exec();
}
void MainWindowPersonal::on_actionMailListe_triggered()
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


void MainWindowPersonal::on_actionMitgliederEinzelListePDF_triggered()
{
    Export::Mitglieder::printMitgliederEinzelListe(getSortierteListe(), personal, filter,
                        Export::getPrinterPDF(this, "Stammdatenblaetter", QPrinter::Orientation::Portrait));
}
void MainWindowPersonal::on_actionMitgliederEinzelListeDrucken_triggered()
{
    Export::Mitglieder::printMitgliederEinzelListe(getSortierteListe(), personal, filter,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}

void MainWindowPersonal::on_actionMitgliederListePDF_triggered()
{
    Export::Mitglieder::printMitgliederListe(getSortierteListe(), filter, anzeige,
                            Export::getPrinterPDF(this, "Mitgliederliste", QPrinter::Orientation::Portrait));
}
void MainWindowPersonal::on_actionMitgliederListeDrucken_triggered()
{
    Export::Mitglieder::printMitgliederListe(getSortierteListe(), filter, anzeige,
                            Export::getPrinterPaper(this, QPrinter::Orientation::Landscape));
}
void MainWindowPersonal::on_actionMitgliederListeCSV_triggered()
{
    Export::Mitglieder::exportMitgliederAlsCSV(current,
                                   FileIO::getFilePathSave(this, "Mitglieder", FileIO::DateiTyp::CSV));
}


void MainWindowPersonal::on_comboAnzeige_currentIndexChanged(int index)
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
    on_actionAktualisieren_triggered();
}

void MainWindowPersonal::on_tabelleMitglieder_cellDoubleClicked(int row, [[maybe_unused]] int column)
{
    PersonTableWidgetItem *clicked = dynamic_cast<PersonTableWidgetItem*>(ui->tabelleMitglieder->item(row, column));
    if (clicked != nullptr) {
        showPerson(clicked->getPerson());
    }
//    QString name = ui->tabelleMitglieder->item(row, 0)->text() + " " + ui->tabelleMitglieder->item(row, 1)->text();
//    Person * p = personal->getPerson(name);
//    if (p != nullptr) {
//        showPerson(p);
//    }
}


void MainWindowPersonal::showPerson(Person *p)
{
    if (p == nullptr) return;

    if (fenster.contains(p)) {
        fenster.value(p)->show();
        fenster.value(p)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster.value(p)->raise();  // for MacOS
        fenster.value(p)->activateWindow(); // for Windows
    } else {
        PersonWindow *w = new PersonWindow(this, p);
        w->setWindowFilePath(datei->getPfad());
        fenster.insert(p, w);
        w->show();
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

void MainWindowPersonal::on_listAnzeige_itemChanged(QListWidgetItem *item)
{
    if (item->checkState() == Qt::CheckState::Checked) {
        anzeige.insert(item->text());
    } else {
        anzeige.remove(item->text());
    }
    on_actionAktualisieren_triggered();
}
