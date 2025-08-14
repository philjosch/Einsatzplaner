#include "mainwindowpersonal.h"
#include "ui_mainwindowpersonal.h"

#include "fileio.h"
#include "minimumhourseditordialog.h"
#include "export.h"
#include "guihelper.h"
#include "beitraegeeditordialog.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QTreeWidget>

MainWindowPersonal::MainWindowPersonal(EplFile *file) :
    CoreMainWindow(file), ui(new Ui::MainWindowPersonal)
{
    ui->setupUi(this);

    comboAnzeige = new QComboBox();
    comboAnzeige->addItem(toString(Status::AlleMitglieder), QVariant(Status::AlleMitglieder));
    comboAnzeige->addItem(toString(Status::Aktiv), QVariant(Status::Aktiv));
    comboAnzeige->addItem(toString(Status::AktivMit), QVariant(Status::AktivMit));
    comboAnzeige->addItem(toString(Status::AktivOhne), QVariant(Status::AktivOhne));
    comboAnzeige->addItem(toString(Status::Passiv), QVariant(Status::Passiv));
    comboAnzeige->addItem(toString(Status::PassivMit), QVariant(Status::PassivMit));
    comboAnzeige->addItem(toString(Status::Ausgetreten), QVariant(Status::Ausgetreten));
    comboAnzeige->addItem(toString(Status::Registriert), QVariant(Status::Registriert));
    comboAnzeige->setMinimumSize(QSize(150, 0));
    ui->mainToolBar->insertWidget(ui->actionAktualisieren, comboAnzeige);
    QWidget *strechWidget1 = new QWidget();
    strechWidget1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->mainToolBar->insertWidget(ui->actionMailListe, strechWidget1);


    QTreeWidgetItem *__treeGeneral = new QTreeWidgetItem(ui->treeDisplaySelection);
    __treeGeneral->setData(0, Qt::UserRole, QVariant());
    __treeGeneral->setText(0, tr("Persönliches"));
    QTreeWidgetItem *__treeGeneralFirstname = new QTreeWidgetItem(__treeGeneral);
    __treeGeneralFirstname->setData(0, Qt::UserRole, QVariant("Vorname"));
    __treeGeneralFirstname->setText(0, tr("Vorname"));
    QTreeWidgetItem *__treeGeneralLastName = new QTreeWidgetItem(__treeGeneral);
    __treeGeneralLastName->setData(0, Qt::UserRole, QVariant("Nachname"));
    __treeGeneralLastName->setText(0, tr("Nachname"));
    QTreeWidgetItem *__treeGeneralBirthdate = new QTreeWidgetItem(__treeGeneral);
    __treeGeneralBirthdate->setData(0, Qt::UserRole, QVariant("Geburtsdatum"));
    __treeGeneralBirthdate->setText(0, tr("Geburtstag"));
    QTreeWidgetItem *__treeGeneralGender = new QTreeWidgetItem(__treeGeneral);
    __treeGeneralGender->setData(0, Qt::UserRole, QVariant("Geschlecht"));
    __treeGeneralGender->setText(0, tr("Geschlecht"));
    QTreeWidgetItem *__treeGeneralGreeting = new QTreeWidgetItem(__treeGeneral);
    __treeGeneralGreeting->setData(0, Qt::UserRole, QVariant("Anrede"));
    __treeGeneralGreeting->setText(0, tr("Anrede"));
    QTreeWidgetItem *__treeGeneralOccupation = new QTreeWidgetItem(__treeGeneral);
    __treeGeneralOccupation->setData(0, Qt::UserRole, QVariant("Beruf"));
    __treeGeneralOccupation->setText(0, tr("Beruf"));

    QTreeWidgetItem *__treeMembership = new QTreeWidgetItem(ui->treeDisplaySelection);
    __treeMembership->setData(0, Qt::UserRole, QVariant());
    __treeMembership->setText(0, tr("Mitgliedschaft"));
    QTreeWidgetItem *__treeMembershipNumber = new QTreeWidgetItem(__treeMembership);
    __treeMembershipNumber->setData(0, Qt::UserRole, QVariant("Nummer"));
    __treeMembershipNumber->setText(0, tr("Nummer"));
    QTreeWidgetItem *__treeMembershipJoindate = new QTreeWidgetItem(__treeMembership);
    __treeMembershipJoindate->setData(0, Qt::UserRole, QVariant("Eintritt"));
    __treeMembershipJoindate->setText(0, tr("Eintritt"));
    QTreeWidgetItem *__treeMembershipStatus = new QTreeWidgetItem(__treeMembership);
    __treeMembershipStatus->setData(0, Qt::UserRole, QVariant("Status"));
    __treeMembershipStatus->setText(0, tr("Status"));
    QTreeWidgetItem *__treeMembershipLeavedate = new QTreeWidgetItem(__treeMembership);
    __treeMembershipLeavedate->setData(0, Qt::UserRole, QVariant("Austritt"));
    __treeMembershipLeavedate->setText(0, tr("Austritt"));
    QTreeWidgetItem *__treeMembershipFeetype = new QTreeWidgetItem(__treeMembership);
    __treeMembershipFeetype->setData(0, Qt::UserRole, QVariant("Beitragsart"));
    __treeMembershipFeetype->setText(0, tr("Beitragsart"));
    QTreeWidgetItem *__treeMembershipIban = new QTreeWidgetItem(__treeMembership);
    __treeMembershipIban->setData(0, Qt::UserRole, QVariant("IBAN"));
    __treeMembershipIban->setText(0, tr("IBAN"));
    QTreeWidgetItem *__treeMembershipBank = new QTreeWidgetItem(__treeMembership);
    __treeMembershipBank->setData(0, Qt::UserRole, QVariant("Bank"));
    __treeMembershipBank->setText(0, tr("Bank"));
    QTreeWidgetItem *__treeMembershipHolder = new QTreeWidgetItem(__treeMembership);
    __treeMembershipHolder->setData(0, Qt::UserRole, QVariant("Kontoinhaber"));
    __treeMembershipHolder->setText(0, tr("Kontoinhaber"));
    QTreeWidgetItem *__treeMembershipFee = new QTreeWidgetItem(__treeMembership);
    __treeMembershipFee->setData(0, Qt::UserRole, QVariant("Beitrag"));
    __treeMembershipFee->setText(0, tr("Beitrag"));
    QTreeWidgetItem *__treeMembershipFeeaditional = new QTreeWidgetItem(__treeMembership);
    __treeMembershipFeeaditional->setData(0, Qt::UserRole, QVariant("Beitrag (Nachzahlung)"));
    __treeMembershipFeeaditional->setText(0, tr("Beitrag (Nachzahlung)"));

    QTreeWidgetItem *__treeAddress = new QTreeWidgetItem(ui->treeDisplaySelection);
    __treeAddress->setData(0, Qt::UserRole, QVariant());
    __treeAddress->setText(0, tr("Adresse"));
    QTreeWidgetItem *__treeAddressStreet = new QTreeWidgetItem(__treeAddress);
    __treeAddressStreet->setData(0, Qt::UserRole, QVariant("Straße"));
    __treeAddressStreet->setText(0, tr("Straße"));
    QTreeWidgetItem *__treeAddressPostalcode = new QTreeWidgetItem(__treeAddress);
    __treeAddressPostalcode->setData(0, Qt::UserRole, QVariant("PLZ"));
    __treeAddressPostalcode->setText(0, tr("PLZ"));
    QTreeWidgetItem *__treeAddressCity = new QTreeWidgetItem(__treeAddress);
    __treeAddressCity->setData(0, Qt::UserRole, QVariant("Ort"));
    __treeAddressCity->setText(0, tr("Ort"));
    QTreeWidgetItem *__treeAddressDistance = new QTreeWidgetItem(__treeAddress);
    __treeAddressDistance->setData(0, Qt::UserRole, QVariant("Strecke"));
    __treeAddressDistance->setText(0, tr("Strecke"));

    QTreeWidgetItem *__treeContact = new QTreeWidgetItem(ui->treeDisplaySelection);
    __treeContact->setData(0, Qt::UserRole, QVariant());
    __treeContact->setText(0, tr("Kontakt"));
    QTreeWidgetItem *__treeContactMail = new QTreeWidgetItem(__treeContact);
    __treeContactMail->setData(0, Qt::UserRole, QVariant("Mail"));
    __treeContactMail->setText(0, tr("E-Mail"));
    QTreeWidgetItem *__treeContactMailpermission = new QTreeWidgetItem(__treeContact);
    __treeContactMailpermission->setData(0, Qt::UserRole, QVariant("Mail Zustimmung"));
    __treeContactMailpermission->setText(0, tr("Freigabe E-Mail"));
    QTreeWidgetItem *__treeContactPhone = new QTreeWidgetItem(__treeContact);
    __treeContactPhone->setData(0, Qt::UserRole, QVariant("Telefon"));
    __treeContactPhone->setText(0, tr("Telefon"));
    QTreeWidgetItem *__treeContactPhonetwo = new QTreeWidgetItem(__treeContact);
    __treeContactPhonetwo->setData(0, Qt::UserRole, QVariant("Telefon2"));
    __treeContactPhonetwo->setText(0, tr("2. Telefon"));
    QTreeWidgetItem *__treeContactPhonepermission = new QTreeWidgetItem(__treeContact);
    __treeContactPhonepermission->setData(0, Qt::UserRole, QVariant("Telefon Zustimmung"));
    __treeContactPhonepermission->setText(0, tr("Freigabe Telefon"));

    QTreeWidgetItem *__treeOperation = new QTreeWidgetItem(ui->treeDisplaySelection);
    __treeOperation->setData(0, Qt::UserRole, QVariant());
    __treeOperation->setText(0, tr("Betriebsdienst"));
    QTreeWidgetItem *__treeOperationDriver = new QTreeWidgetItem(__treeOperation);
    __treeOperationDriver->setData(0, Qt::UserRole, QVariant("Tf"));
    __treeOperationDriver->setText(0, tr("Tf"));
    QTreeWidgetItem *__treeOperationConductor = new QTreeWidgetItem(__treeOperation);
    __treeOperationConductor->setData(0, Qt::UserRole, QVariant("Zf"));
    __treeOperationConductor->setText(0, tr("Zf"));
    QTreeWidgetItem *__treeOperationShunter = new QTreeWidgetItem(__treeOperation);
    __treeOperationShunter->setData(0, Qt::UserRole, QVariant("Rangierer"));
    __treeOperationShunter->setText(0, tr("Rangierer:in"));
    QTreeWidgetItem *__treeOperationFitness = new QTreeWidgetItem(__treeOperation);
    __treeOperationFitness->setData(0, Qt::UserRole, QVariant("Tauglichkeit"));
    __treeOperationFitness->setText(0, tr("Tauglichkeit"));
    QTreeWidgetItem *__treeOperationComments = new QTreeWidgetItem(__treeOperation);
    __treeOperationComments->setData(0, Qt::UserRole, QVariant("Bemerkung Betrieb."));
    __treeOperationComments->setText(0, tr("Betriebl. Bemerkungen"));
    QTreeWidgetItem *__treeOperationAdditional = new QTreeWidgetItem(__treeOperation);
    __treeOperationAdditional->setData(0, Qt::UserRole, QVariant("Sonst. Ausbildung"));
    __treeOperationAdditional->setText(0, tr("Sonstige Ausbildung"));

    QTreeWidgetItem *__treeComments = new QTreeWidgetItem(ui->treeDisplaySelection);
    __treeComments->setData(0, Qt::UserRole, QVariant("Bemerkung"));
    __treeComments->setText(0, tr("Bemerkungen"));

    QTreeWidgetItemIterator treeIterator = QTreeWidgetItemIterator(ui->treeDisplaySelection);
    while (*treeIterator) {
        (*treeIterator)->setCheckState(0, Qt::Unchecked);
        (*treeIterator)->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsAutoTristate);
        (*treeIterator)->setExpanded(true);
        treeIterator++;
    }

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

    connect(comboAnzeige, &QComboBox::currentIndexChanged, this, &MainWindowPersonal::filterChanged);
    connect(ui->tabelleMitglieder, &QTableWidget::cellDoubleClicked, this, &MainWindowPersonal::showPersFromTable);

    connect(ui->treeDisplaySelection, &QTreeWidget::itemChanged, this, &MainWindowPersonal::updateTableBasedOnCategorySelection);

    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    current = QList<Person*>();
    filter = Status::AlleMitglieder;
    anzeige = QSet<QString>();


    __treeGeneralFirstname->setCheckState(0, Qt::CheckState::Checked);
    __treeGeneralLastName->setCheckState(0, Qt::CheckState::Checked);
    __treeMembershipNumber->setCheckState(0, Qt::CheckState::Checked);
    __treeMembershipJoindate->setCheckState(0, Qt::CheckState::Checked);
    __treeMembershipStatus->setCheckState(0, Qt::CheckState::Checked);
    __treeAddressStreet->setCheckState(0, Qt::CheckState::Checked);
    __treeAddressPostalcode->setCheckState(0, Qt::CheckState::Checked);
    __treeAddressCity->setCheckState(0, Qt::CheckState::Checked);
    __treeContactMail->setCheckState(0, Qt::CheckState::Checked);
    __treeContactPhone->setCheckState(0, Qt::CheckState::Checked);
    __treeContactPhonetwo->setCheckState(0, Qt::CheckState::Checked);
    __treeComments->setCheckState(0, Qt::CheckState::Checked);

    comboAnzeige->setCurrentIndex(0);

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
    for (Person *p: std::as_const(current)) {
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
        if (anzeige.contains("Mail Zustimmung"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getMailOK() ? "Ja": "Nein"));
        if (anzeige.contains("Telefon"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getTelefon()));
        if (anzeige.contains("Telefon2"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getTelefon2()));
        if (anzeige.contains("Telefon Zustimmung"))
            ui->tabelleMitglieder->setItem(0, clmn++, new PersonTableWidgetItem(p, p->getTelefonOK() ? "Ja" : "Nein"));


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
    for (Person *p: std::as_const(current)) {
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

    if (QMessageBox::question(this,
                              tr("Personen ohne E-Mail gefunden"),
                              tr("Für %1 %2 ist keine E-Mail-Adresse hinterlegt.\nMöchten Sie die Adressen dieser Personen speichern?").arg(keineMail.size()).arg(toString(filter)))
            == QMessageBox::Yes) {
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
    personal->exportMembersSingleAsHtml(Export::getPrinterPDF(this, "Stammdaten", QPageLayout::Orientation::Portrait),
                                    getSortierteListe(), filter);
}
void MainWindowPersonal::exportMemberDetailMultiplePrint()
{
    personal->exportMembersSingleAsHtml(Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait),
                                    getSortierteListe(), filter);
}

void MainWindowPersonal::exportMemberListPdf()
{
    personal->exportMembersListAsHtml(Export::getPrinterPDF(this, "Mitgliederliste", QPageLayout::Orientation::Landscape),
                                   getSortierteListe(), filter, anzeige);
}
void MainWindowPersonal::exportMemberListPrint()
{
    personal->exportMembersListAsHtml(Export::getPrinterPaper(this, QPageLayout::Orientation::Landscape),
                                   getSortierteListe(), filter, anzeige);
}
void MainWindowPersonal::exportMemberListCsv()
{
    personal->exportMembersListAsCsv(FileIO::getFilePathSave(this, "Mitgliederliste", FileIO::DateiTyp::CSV),
                                        getSortierteListe(), anzeige);
}

void MainWindowPersonal::exportDuesRegularCsv()
{
    personal->saveBeitraegeRegulaerAlsCSV(FileIO::getFilePathSave(this, "Beitraege-Regulaer", FileIO::DateiTyp::CSV));
}
void MainWindowPersonal::exportDuesAdditionalCsv()
{
    personal->saveBeitraegeNachzahlungAlsCSV(FileIO::getFilePathSave(this, "Beitraege-Nachzahlung", FileIO::DateiTyp::CSV));
}

void MainWindowPersonal::filterChanged()
{
    filter = Status(comboAnzeige->currentData().toInt());
    refresh();
}

void MainWindowPersonal::showPersFromTable(int row, [[maybe_unused]] int column)
{
    PersonTableWidgetItem *clicked = dynamic_cast<PersonTableWidgetItem*>(ui->tabelleMitglieder->item(row, column));
    if (clicked != nullptr) {
        openPerson(clicked->getPerson());
    }
}

void MainWindowPersonal::updateTableBasedOnCategorySelection(QTreeWidgetItem *item, [[maybe_unused]] int column)
{
    if (item->childCount() > 0) return;
    QString role = item->data(0, Qt::UserRole).toString();
    if (role == "") return;
    if (item->checkState(0) == Qt::CheckState::Checked)
        anzeige.insert(role);
    else
        anzeige.remove(role);

    refresh();
}


QList<Person*> MainWindowPersonal::getSortierteListe()
{
    QList<Person*> liste = QList<Person*>();
    for(int i = 0; i < ui->tabelleMitglieder->rowCount(); i++) {
        PersonTableWidgetItem *ptwi = static_cast<PersonTableWidgetItem*>(ui->tabelleMitglieder->item(i, 0));
        if (ptwi == nullptr)
            continue;
        if (ptwi->getPerson() != nullptr)
            liste.append(ptwi->getPerson());
    }
    return liste;
}

