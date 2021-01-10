#include "mainwindowpersonal.h"
#include "ui_mainwindowpersonal.h"

#include "fileio.h"
#include "coreapplication.h"
#include "minimumhourseditordialog.h"

#include <QDesktopServices>
#include <QJsonArray>
#include <QMessageBox>
#include <eplfile.h>

#include <export.h>
#include <manager.h>
#include <person.h>


MainWindowPersonal::MainWindowPersonal(QWidget *parent) :
    CoreMainWindow(parent), ui(new Ui::MainWindowPersonal)
{
    constructor();
}

MainWindowPersonal::MainWindowPersonal(EplFile *file, QWidget *parent) :
    CoreMainWindow(file, parent), ui(new Ui::MainWindowPersonal)
{
    constructor();
    updateWindowHeaders();
    refresh();
}

void MainWindowPersonal::constructor()
{
    ui->setupUi(this);

    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    current = QList<Person*>();
    filter = Mitglied::Aktiv;

    fenster = QMap<Person*, PersonWindow*>();


    manager = datei->getManager();
    personal = datei->getPersonal();


    connect(personal, SIGNAL(changed()), this, SLOT(unsave()));
    connect(personal, SIGNAL(del(Person *)), this, SLOT(personLoeschen(Person*)));

    connect(ui->actionMindeststunden, SIGNAL(triggered()), this, SLOT(editMinimumHours()));
    connect(ui->comboAnzeige, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));
    connect(ui->actionAktualisieren, SIGNAL(triggered()), this, SLOT(refresh()));
}


MainWindowPersonal::~MainWindowPersonal()
{
    close();
    delete ui;
}

bool MainWindowPersonal::open(QString path)
{
    EplFile* datei = CoreMainWindow::open(path);
    if (datei == nullptr) return false;

    CoreMainWindow *mw = new MainWindowPersonal(datei);
    mw->show();
    return true;
}

CoreMainWindow *MainWindowPersonal::handlerNew()
{
    return new MainWindowPersonal();
}

void MainWindowPersonal::handlerPrepareSave()
{
    // TODO
}

void MainWindowPersonal::handlerOpen(QString path)
{
    MainWindowPersonal::open(path);
}

void MainWindowPersonal::refresh()
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
    current = personal->getPersonen(filter);


    // Tabelle leeren
    ui->tabelleMitglieder->setRowCount(0);
    ui->tabelleMitglieder->setSortingEnabled(false);

    int clmn;
    QTableWidgetItem *i;
    for (Person *p: current) {
        clmn = 0;
        ui->tabelleMitglieder->insertRow(0);


        i = new QTableWidgetItem(p->getVorname());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        i = new QTableWidgetItem(p->getNachname());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        i = new QTableWidgetItem();
        i->setData(0, p->getNummer());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        i = new QTableWidgetItem(
                    QString("%1%2")
                    .arg(p->isAusgetreten() ? "Ehemals ": "")
                    .arg(p->getAktiv() ? "Aktiv" : "Passiv"));
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        i = new QTableWidgetItem();
        i->setData(0, p->getGeburtstag());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        i = new QTableWidgetItem();
        i->setData(0, p->getEintritt());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        if (p->isAusgetreten() || p->getAustritt().isValid()) {
            i = new QTableWidgetItem();
            i->setData(0, p->getAustritt());
            ui->tabelleMitglieder->setItem(0, clmn++, i);
        } else {
            clmn++;
        }

        ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem(p->getBeruf()));

        ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem(p->getStrasse()));
        i = new QTableWidgetItem();
        i->setData(0, p->getPLZ());
        ui->tabelleMitglieder->setItem(0, clmn++, i);
        ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem(p->getOrt()));
        i = new QTableWidgetItem();
        i->setData(0, p->getStrecke());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem(p->getMail()));
        if (p->getMailOK())
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Ja"));
        else
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Nein"));
        ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem(p->getTelefon()));
        if (p->getTelefonOK())
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Ja"));
        else
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Nein"));

        if (p->getAusbildungTf())
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Ja"));
        else
            clmn++;
        if (p->getAusbildungZf())
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Ja"));
        else
            clmn++;
        if (p->getAusbildungRangierer())
            ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem("Ja"));
        else
            clmn++;

        i = new QTableWidgetItem();
        i->setData(0, p->getTauglichkeit());
        ui->tabelleMitglieder->setItem(0, clmn++, i);

        ui->tabelleMitglieder->setItem(0, clmn++, new QTableWidgetItem(p->getBemerkungen().replace("<br/>","\n")));
    }

    ui->tabelleMitglieder->setSortingEnabled(true);
    ui->tabelleMitglieder->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    ui->labelMitgliederAnzMitglieder->setText(QString::number(personal->getAnzahlMitglieder(Mitglied::AlleMitglieder)));
    ui->labelMitgliederAnzAktiv->setText(QString::number(personal->getAnzahlMitglieder(Mitglied::Aktiv)));
    ui->labelMitgliederAnzPassiv->setText(QString::number(personal->getAnzahlMitglieder(Mitglied::Passiv)));
    ui->labelMitgliederAnzAusgetreten->setText(QString::number(personal->getAnzahlMitglieder(Mitglied::Ausgetreten)));
    ui->labelMitgliederAnzErfasst->setText(QString::number(personal->getAnzahlMitglieder(Mitglied::Registriert)));
}

void MainWindowPersonal::editMinimumHours()
{
   MinimumHoursEditorDialog(personal, this).exec();
}

void MainWindowPersonal::on_actionMitgliederEinzelListePDF_triggered()
{
    Export::printMitgliederEinzelListe(getSortierteListe(), manager->getPersonal(), filter,
                        Export::getPrinterPDF(this, "Stammdatenblaetter.pdf", QPrinter::Orientation::Portrait));
}
void MainWindowPersonal::on_actionMitgliederEinzelListeDrucken_triggered()
{
    Export::printMitgliederEinzelListe(getSortierteListe(), manager->getPersonal(), filter,
                        Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}

void MainWindowPersonal::on_actionMitgliederListePDF_triggered()
{
    Export::printMitgliederListe(getSortierteListe(), filter,
                            Export::getPrinterPDF(this, "Mitgliederliste.pdf", QPrinter::Orientation::Portrait));
}
void MainWindowPersonal::on_actionMitgliederListeDrucken_triggered()
{
    Export::printMitgliederListe(getSortierteListe(), filter,
                            Export::getPrinterPaper(this, QPrinter::Orientation::Landscape));
}
void MainWindowPersonal::on_actionMitgliederListeCSV_triggered()
{
    Export::exportMitgliederAlsCSV(current,
                                  FileIO::getFilePathSave(this, "Mitglieder.csv", tr("CSV-Datei (*.csv)")));
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
        QString path = FileIO::getFilePathSave(this, "Adressen.csv", tr("CSV-Datei (*.csv)"));
        FileIO::saveToFile(path, s);
    }
}

void MainWindowPersonal::on_tabelleMitglieder_cellDoubleClicked(int row, [[maybe_unused]] int column)
{
    QString name = ui->tabelleMitglieder->item(row, 0)->text() + " " + ui->tabelleMitglieder->item(row, 1)->text();
    Person * p = personal->getPerson(name);
    if (p != nullptr) {
        showPerson(p);
    }
}

void MainWindowPersonal::on_actionAddPerson_triggered()
{
    Person *neu = manager->getPersonal()->newPerson();
    showPerson(neu);
    refresh();
    unsave();
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

void MainWindowPersonal::personLoeschen(Person *p)
{
    if (personal->removePerson(p)) {
        if (fenster.contains(p)) {
            fenster.remove(p);
        }
        refresh();
    }
}

QList<Person*> MainWindowPersonal::getSortierteListe()
{
    QList<Person*> liste = QList<Person*>();
    for(int i = 0; i < ui->tabelleMitglieder->rowCount(); i++) {
        QString name = ui->tabelleMitglieder->item(i, 0)->text();
        if (name != "") name += " ";
        name += ui->tabelleMitglieder->item(i, 1)->text();
        Person *pers = manager->getPersonal()->getPerson(name);
        if (pers != nullptr)
            liste.append(pers);
    }
    return liste;
}
