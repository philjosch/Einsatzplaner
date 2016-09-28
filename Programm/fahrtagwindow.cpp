#include "fahrtagwindow.h"
#include "person.h"
#include "ui_fahrtagwindow.h"
#include "export.h"

#include <QDebug>
#include <QMessageBox>
#include <QListWidget>

FahrtagWindow::FahrtagWindow(QWidget *parent, Fahrtag *f) : QMainWindow(parent), ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
    ui->lineSitze->setValidator(new QRegExpValidator(QRegExp("2[0-9]{2}\\s*\\:\\s*(((([1-9][0-9]?)\\s*-\\s*([1-9][0-9]?))|([1-9][0-9]?)))(\\s*,\\s*((([1-9][0-9]?)-([1-9][0-9]?))|([1-9][0-9]?)))*(;\\s*[1-9][0-9]?\\s*\\:\\s*(((([1-9][0-9]?)\\s*-\\s*([1-9][0-9]?))|([1-9][0-9]?)))(\\s*,\\s*((([1-9][0-9]?)-([1-9][0-9]?))|([1-9][0-9]?)))*\\s*)*")));

    fahrtag = f;
    nehme = true;
    ui->dateDate->setFocus();
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum().toString("dddd dd. MM. yyyy"));

    listeMitNamen = new QMap<QListWidgetItem *, QString>();
    listToTable = new QMap<QListWidgetItem*,QTableWidgetItem*>();
    namen = new QSet<QString>();

    resToItem = new QMap<Reservierung*, QListWidgetItem*>();
    itemToRes = new QMap<QListWidgetItem*, Reservierung*>();

    loadData();

    nehmeRes = false;

}

FahrtagWindow::~FahrtagWindow()
{
    delete ui;
}

void FahrtagWindow::on_dateDate_dateChanged(const QDate &date)
{
    if (nehme)
        fahrtag->setDatum(date);
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum().toString("dddd dd. MM. yyyy"));
}

void FahrtagWindow::on_comboArt_currentIndexChanged(int index)
{
    Fahrtag::Art art = (Fahrtag::Art) index;
    if (nehme) {
        fahrtag->setArt(art);
    }
    ui->tabReservierungen->setEnabled(art != Fahrtag::Schnupperkurs);
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum().toString("dddd dd. MM. yyyy"));
}

void FahrtagWindow::on_textAnlass_textChanged()
{
    if (nehme)
        fahrtag->setAnlass(ui->textAnlass->toPlainText());
}

void FahrtagWindow::on_checkWichtig_stateChanged(int arg1)
{
    if (nehme)
        fahrtag->setWichtig(arg1 == 1);
}

void FahrtagWindow::on_comboWagenreihung_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setWagenreihung(arg1);
}

void FahrtagWindow::on_comboTimeTfH_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setZeitTf(QTime(arg1.toInt(), fahrtag->getZeitTf().minute()));
}

void FahrtagWindow::on_comboTimeTfM_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setZeitTf(QTime(fahrtag->getZeitTf().hour(), arg1.toInt()));
}

void FahrtagWindow::on_comboTimeZH_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setZeitAnfang(QTime(arg1.toInt(), fahrtag->getZeitAnfang().minute()));
}

void FahrtagWindow::on_comboTimeZM_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setZeitAnfang(QTime(fahrtag->getZeitAnfang().hour(), arg1.toInt()));
}

void FahrtagWindow::on_textBemerkungen_textChanged()
{
    if (nehme)
        fahrtag->setBemerkungen(ui->textBemerkungen->toPlainText());
}

void FahrtagWindow::on_timeEnde_timeChanged(const QTime &time)
{
    if (nehme)
        fahrtag->setZeitEnde(QTime(time.hour(), time.minute()));
}

void FahrtagWindow::loadData()
{
    if (nehme) {
        nehme = false;
        // Allgemeine Daten von AActivity
        ui->dateDate->setDate(fahrtag->getDatum());
        ui->textAnlass->clear();
        ui->textAnlass->insertPlainText(fahrtag->getAnlass());
        ui->timeEnde->setTime(fahrtag->getZeitEnde());
        ui->checkBoxBenoetigt->setChecked(fahrtag->getPersonalBenoetigt());

        // Daten von Manager_Reservierungen
        ui->comboWagenreihung->setCurrentText(fahrtag->getWagenreihung());
        QSetIterator<Reservierung*> iter = fahrtag->getReservierungen();
        while(iter.hasNext()) {
            Reservierung *r = iter.next();
            QListWidgetItem *item = new QListWidgetItem(r->getName());
            resToItem->insert(r, item);
            itemToRes->insert(item, r);
            ui->listRes->insertItem(0, item);
        }
        ui->checkBoxAuto->setChecked(fahrtag->getAutoPlatz());
        ui->checkBoxAll->setChecked(fahrtag->getCheckAll());
        ui->checkBoxAll->setEnabled(fahrtag->getAutoPlatz());
        ui->buttonVerteile->setEnabled(fahrtag->getAutoPlatz());


        // Daten von Fahrtag
        ui->comboArt->setCurrentIndex((int)fahrtag->getArt());
        ui->comboTimeTfH->setCurrentText(fahrtag->getZeitTf().toString("HH"));
        ui->comboTimeTfM->setCurrentText(fahrtag->getZeitTf().toString("mm"));
        ui->comboTimeZH->setCurrentText(fahrtag->getZeitAnfang().toString("HH"));
        ui->comboTimeZM->setCurrentText(fahrtag->getZeitAnfang().toString("mm"));
        ui->checkWichtig->setChecked(fahrtag->getWichtig());
        ui->checkTf->setChecked(fahrtag->getBenoetigeTf());
        ui->checkZf->setChecked(fahrtag->getBenoetigeZf());
        ui->checkZub->setChecked(fahrtag->getBenoetigeZub());
        ui->checkService->setChecked(fahrtag->getBenoetigeService());

        /* LIstTf, ListZf, ListZub, ListService fehlt noch */
        QMap<Person*, AActivity::Infos*> tf;
        QMap<Person*, AActivity::Infos*> zf;
        QMap<Person*, AActivity::Infos*> zub;
        QMap<Person*, AActivity::Infos*> service;
        QMap<Person*, AActivity::Infos*> sonstige;

        listeMitNamen = new QMap<QListWidgetItem*, QString>();
        listToTable = new QMap<QListWidgetItem*, QTableWidgetItem*>();
        namen = new QSet<QString>();


        // Aufsplitten der Personen auf die Einzelnen Listen
        for(Person *p: fahrtag->getPersonen()->keys()) {
            AActivity::Infos *info = fahrtag->getPersonen()->value(p);

            QListWidgetItem *item;
            if (info->bemerkung == "") {
                item = new QListWidgetItem(p->getName());
            } else {
                item = new QListWidgetItem(p->getName()+"; "+info->bemerkung);
            }
            listeMitNamen->insert(item, p->getName());
            namen->insert(p->getName());

            bool block = true;

            switch (fahrtag->getPersonen()->value(p)->kategorie) {
            case AActivity::Tf:
            case AActivity::Tb:
                tf.insert(p, info);
                ui->listTf->insertItem(0, item);
                ui->buttonTfDelete->setEnabled(true);
                break;
            case AActivity::Zf:
                zf.insert(p, info);
                ui->listZf->insertItem(0, item);
                ui->buttonZfDelete->setEnabled(true);
                break;
            case AActivity::Zub:
            case AActivity::Begleiter:
                zub.insert(p, info);
                ui->listZub->insertItem(0, item);
                ui->buttonZubDelete->setEnabled(true);
                break;
            case AActivity::Service:
                service.insert(p, info);
                ui->listService->insertItem(0, item);
                ui->buttonServiceDelete->setEnabled(true);
                break;
            default:
                sonstige.insert(p, info);
                block = false;
                ui->buttonRemove->setEnabled(true);
                break;
            }

            // Zeile für die Person in die Tabelle einfügen
            QString bem = info->bemerkung.toUpper();
            if ( ! (bem.contains("EXTERN") || ((bem.contains("AZUBI") || bem.contains("FS")) && (info->kategorie == AActivity::Tf  || info->kategorie == AActivity::Tb)))) {

                ui->tablePersonen->insertRow(0);

                QTableWidgetItem *zelleName = new QTableWidgetItem(p->getName());
                if (block) zelleName->setFlags(Qt::NoItemFlags);
                ui->tablePersonen->setItem(0,0,zelleName);

                if (info->beginn != QTime(0, 0)) {
                    QTableWidgetItem *zelleBeginn = new QTableWidgetItem(info->beginn.toString("hh:mm"));
                    ui->tablePersonen->setItem(0, 1, zelleBeginn);
                }
                if (info->ende != QTime(0,0)) {
                    QTableWidgetItem *zelleEnde = new QTableWidgetItem(info->ende.toString("hh:mm"));
                    ui->tablePersonen->setItem(0, 2, zelleEnde);
                }
                QTableWidgetItem *zelleAufgabe;
                if (info->aufgabe == "") {
                    zelleAufgabe = new QTableWidgetItem(AActivity::getStringFromCategory(info->kategorie));
                } else {
                    zelleAufgabe = new QTableWidgetItem(info->aufgabe);
                }
                if (block) zelleAufgabe->setFlags(Qt::NoItemFlags);
                ui->tablePersonen->setItem(0, 3, zelleAufgabe);
                listToTable->insert(item, zelleName);
            }
        }
        nehme = true;
    }
}


void FahrtagWindow::addItemTolist(QListWidget *l, QPushButton *b)
{
    if (nehme) {
        nehme = false;
        l->addItem("(Name eingeben)");
        l->item(l->count()-1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        b->setEnabled(true);
        nehme = true;
    }
}
void FahrtagWindow::deleteItemFromList(QListWidget *l, QPushButton *b)
{
    l->takeItem(l->currentRow());
    b->setEnabled(l->count()>0);
}


void FahrtagWindow::on_listTf_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        // Laden der Informationen zur geänderten Person
        QString text = item->text();
        QStringList liste = text.split("; ");
        QString name = liste.at(0);
        QString bem = "";
        if (liste.length() > 1) {
            bem = liste.at(1);
        }
        AActivity::Category kat = AActivity::Tf;
        if (bem.contains("Tb") || bem.contains("TB")) {
            kat = AActivity::Tb;
        }
        Person *person;

        if (bem.contains("Azubi") || bem.contains("FS") || bem.contains("Extern")) {
            person = new Person(name);
            person->setAusbildungTf(true);
            kat = AActivity::Tf;
            person->addActivity(fahrtag, kat);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), "Tf");

            if (listeMitNamen->contains(item)) {
                fahrtag->removePerson(listeMitNamen->value(item));
                listeMitNamen->remove(item);
                ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
                namen->remove(name);
            }
            nehme = true;
            return;
        } else if (!fahrtag->getPersonal()->personExists(name)) {
            QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
            nehme = true;
            return;
        } else {
            person = fahrtag->getPersonal()->getPerson(name);
            // Prüfen, ob die Person die Qualifikationen hat
            if (! person->getAusbildungTf()) {
                QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
                nehme = true;
                return;
            }
        }
        // Die Person ist in Ordnung und kann jetzt übernommen werden
        name = person->getName();
        person->addActivity(fahrtag, kat);

        if (listeMitNamen->contains(item)) {
            // Zeile war schon vorhanden und kann jetzt angepasst werden
            QString nameAlt = listeMitNamen->value(item);
            fahrtag->removePerson(nameAlt); // Alte Person von Fahrtag lösen
            listeMitNamen->insert(item, name); // den neuen Namen der Person speichern, der alte wird jetzt nicht mehr benötigt
            listToTable->value(item)->setText(name); // der Name der person in der Tabelle wird angepasst
        } else {
            listeMitNamen->insert(item, name);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), AActivity::getStringFromCategory(kat));
            // Zeile für die Person in die Tabelle einfügen
            ui->tablePersonen->insertRow(0);
            QTableWidgetItem *zelleName = new QTableWidgetItem(name);
            zelleName->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,0,zelleName);
            QTableWidgetItem *zelleAufgabe = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
            zelleAufgabe->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,3,zelleAufgabe);
            listToTable->insert(item, zelleName);
            namen->insert(name);
        }
        nehme = true;
    }
}
void FahrtagWindow::on_buttonTfAdd_clicked()
{
    addItemTolist(ui->listTf, ui->buttonTfDelete);
}
void FahrtagWindow::on_buttonTfDelete_clicked()
{
    QString text = ui->listTf->currentItem()->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }

    if (bem.contains("Azubi") || bem.contains("FS") || bem.contains("Extern")) {
        fahrtag->removePerson(name);
    } else if (!fahrtag->getPersonal()->personExists(name)) {
    } else {

        Person *person = fahrtag->getPersonal()->getPerson(name);
        name = person->getName();
        person->removeActivity(fahrtag);
    }
    QListWidgetItem *item = ui->listTf->currentItem();
    if (listeMitNamen->contains(item)) {
        listeMitNamen->remove(item);
    }
    if (listToTable->contains(item)) {
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
        listToTable->remove(item);
    }
    deleteItemFromList(ui->listTf, ui->buttonTfDelete);

}
void FahrtagWindow::on_checkTf_clicked(bool checked)
{
    fahrtag->setBenoetigeTf(checked);
}

void FahrtagWindow::on_listZf_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        // Laden der Informationen zur geänderten Person
        QString text = item->text();
        QStringList liste = text.split("; ");
        QString name = liste.at(0);
        QString bem = "";
        if (liste.length() > 1) {
            bem = liste.at(1);
        }
        AActivity::Category kat = AActivity::Zf;
        Person *person;

        if (bem.contains("Extern")) {
            person = new Person(name);
            person->setAusbildungZf(true);
            person->addActivity(fahrtag, kat);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), "Zf");

            if (listeMitNamen->contains(item)) {
                fahrtag->removePerson(listeMitNamen->value(item));
                listeMitNamen->remove(item);
                ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
                namen->remove(name);
            }
            nehme = true;
            return;
        } else if (!fahrtag->getPersonal()->personExists(name)) {
            QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
            nehme = true;
            return;
        } else {
            person = fahrtag->getPersonal()->getPerson(name);
            // Prüfen, ob die Person die Qualifikationen hat
            if (! person->getAusbildungZf()) {
                QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
                nehme = true;
                return;
            }
        }
        // Die Person ist in Ordnung und kann jetzt übernommen werden
        name = person->getName();
        person->addActivity(fahrtag, kat);

        if (listeMitNamen->contains(item)) {
            // Zeile war schon vorhanden und kann jetzt angepasst werden
            QString nameAlt = listeMitNamen->value(item);
            fahrtag->removePerson(nameAlt); // Alte Person von Fahrtag lösen
            listeMitNamen->insert(item, name); // den neuen Namen der Person speichern, der alte wird jetzt nicht mehr benötigt
            listToTable->value(item)->setText(name); // der Name der person in der Tabelle wird angepasst
        } else {
            listeMitNamen->insert(item, name);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), AActivity::getStringFromCategory(kat));
            // Zeile für die Person in die Tabelle einfügen
            ui->tablePersonen->insertRow(0);
            QTableWidgetItem *zelleName = new QTableWidgetItem(name);
            zelleName->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,0,zelleName);
            QTableWidgetItem *zelleAufgabe = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
            zelleAufgabe->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,3,zelleAufgabe);
            listToTable->insert(item, zelleName);
            namen->insert(name);
        }
        nehme = true;
    }
}
void FahrtagWindow::on_buttonZfAdd_clicked()
{
    addItemTolist(ui->listZf, ui->buttonZfDelete);
}
void FahrtagWindow::on_buttonZfDelete_clicked()
{
    QString text = ui->listZf->currentItem()->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }

    if (bem.contains("Extern")) {
    } else if (!fahrtag->getPersonal()->personExists(name)) {
    } else {

        Person *person = fahrtag->getPersonal()->getPerson(name);
        name = person->getName();
        person->removeActivity(fahrtag);
    }
    QListWidgetItem *item = ui->listZf->currentItem();
    if (listeMitNamen->contains(item)) {
        listeMitNamen->remove(item);
    }
    if (listToTable->contains(item)) {
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
        listToTable->remove(item);
    }
    deleteItemFromList(ui->listZf, ui->buttonZfDelete);
}
void FahrtagWindow::on_checkZf_clicked(bool checked)
{
    fahrtag->setBenoetigeZf(checked);
}

void FahrtagWindow::on_listZub_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        // Laden der Informationen zur geänderten Person
        QString text = item->text();
        QStringList liste = text.split("; ");
        QString name = liste.at(0);
        QString bem = "";
        if (liste.length() > 1) {
            bem = liste.at(1);
        }
        AActivity::Category kat = AActivity::Zub;
        Person *person;

        if (bem.contains("Extern")) {
            person = new Person(name);
            person->addActivity(fahrtag, kat);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), "Zub");

            if (listeMitNamen->contains(item)) {
                fahrtag->removePerson(listeMitNamen->value(item));
                listeMitNamen->remove(item);
                ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
                namen->remove(name);
            }
            nehme = true;
            return;
        } else if (!fahrtag->getPersonal()->personExists(name)) {
            QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
            nehme = true;
            return;
        } else {
            person = fahrtag->getPersonal()->getPerson(name);
            // Prüfen, ob die Person die Qualifikationen hat
            if (! person->getAusbildungRangierer()) {
                kat = AActivity::Begleiter;
            }
        }
        // Die Person ist in Ordnung und kann jetzt übernommen werden
        name = person->getName();
        person->addActivity(fahrtag, kat);

        if (listeMitNamen->contains(item)) {
            // Zeile war schon vorhanden und kann jetzt angepasst werden
            QString nameAlt = listeMitNamen->value(item);
            fahrtag->removePerson(nameAlt); // Alte Person von Fahrtag lösen
            listeMitNamen->insert(item, name); // den neuen Namen der Person speichern, der alte wird jetzt nicht mehr benötigt
            listToTable->value(item)->setText(name); // der Name der person in der Tabelle wird angepasst
        } else {
            listeMitNamen->insert(item, name);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), AActivity::getStringFromCategory(kat));
            // Zeile für die Person in die Tabelle einfügen
            ui->tablePersonen->insertRow(0);
            QTableWidgetItem *zelleName = new QTableWidgetItem(name);
            zelleName->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,0,zelleName);
            QTableWidgetItem *zelleAufgabe = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
            zelleAufgabe->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,3,zelleAufgabe);
            listToTable->insert(item, zelleName);
            namen->insert(name);
        }
        nehme = true;
    }
}
void FahrtagWindow::on_buttonZubAdd_clicked()
{
    addItemTolist(ui->listZub, ui->buttonZubDelete);
}
void FahrtagWindow::on_buttonZubDelete_clicked()
{
    QString text = ui->listZub->currentItem()->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }

    if (bem.contains("Extern")) {
    } else if (!fahrtag->getPersonal()->personExists(name)) {
    } else {

        Person *person = fahrtag->getPersonal()->getPerson(name);
        name = person->getName();
        person->removeActivity(fahrtag);
    }
    QListWidgetItem *item = ui->listZub->currentItem();
    if (listeMitNamen->contains(item)) {
        listeMitNamen->remove(item);
    }
    if (listToTable->contains(item)) {
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
        listToTable->remove(item);
    }
    deleteItemFromList(ui->listZub, ui->buttonZubDelete);
}
void FahrtagWindow::on_checkZub_clicked(bool checked)
{
    fahrtag->setBenoetigeZub(checked);
}

void FahrtagWindow::on_listService_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        // Laden der Informationen zur geänderten Person
        QString text = item->text();
        QStringList liste = text.split("; ");
        QString name = liste.at(0);
        QString bem = "";
        if (liste.length() > 1) {
            bem = liste.at(1);
        }
        AActivity::Category kat = AActivity::Service;
        Person *person;

        if (bem.contains("Extern")) {
            person = new Person(name);
            person->addActivity(fahrtag, kat);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), "Service");

            if (listeMitNamen->contains(item)) {
                fahrtag->removePerson(listeMitNamen->value(item));
                listeMitNamen->remove(item);
                ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
                namen->remove(name);
            }
            nehme = true;
            return;
        } else if (!fahrtag->getPersonal()->personExists(name)) {
            QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
            nehme = true;
            return;
        } else {
            person = fahrtag->getPersonal()->getPerson(name);
        }
        // Die Person ist in Ordnung und kann jetzt übernommen werden
        name = person->getName();
        person->addActivity(fahrtag, kat);

        if (listeMitNamen->contains(item)) {
            // Zeile war schon vorhanden und kann jetzt angepasst werden
            QString nameAlt = listeMitNamen->value(item);
            fahrtag->removePerson(nameAlt); // Alte Person von Fahrtag lösen
            listeMitNamen->insert(item, name); // den neuen Namen der Person speichern, der alte wird jetzt nicht mehr benötigt
            listToTable->value(item)->setText(name); // der Name der person in der Tabelle wird angepasst
        } else {
            listeMitNamen->insert(item, name);
            fahrtag->addPerson(person, bem, QTime(0,0), QTime(0,0), AActivity::getStringFromCategory(kat));
            // Zeile für die Person in die Tabelle einfügen
            ui->tablePersonen->insertRow(0);
            QTableWidgetItem *zelleName = new QTableWidgetItem(name);
            zelleName->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,0,zelleName);
            QTableWidgetItem *zelleAufgabe = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
            zelleAufgabe->setFlags(Qt::NoItemFlags);
            ui->tablePersonen->setItem(0,3,zelleAufgabe);
            listToTable->insert(item, zelleName);
            namen->insert(name);
        }
        nehme = true;
    }
}
void FahrtagWindow::on_buttonServiceAdd_clicked()
{
    addItemTolist(ui->listService, ui->buttonServiceDelete);
}
void FahrtagWindow::on_buttonServiceDelete_clicked()
{
    QString text = ui->listService->currentItem()->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }

    if (bem.contains("Extern")) {
    } else if (!fahrtag->getPersonal()->personExists(name)) {
    } else {

        Person *person = fahrtag->getPersonal()->getPerson(name);
        name = person->getName();
        person->removeActivity(fahrtag);
    }
    QListWidgetItem *item = ui->listService->currentItem();
    if (listeMitNamen->contains(item)) {
        listeMitNamen->remove(item);
    }
    if (listToTable->contains(item)) {
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
        listToTable->remove(item);
    }
    deleteItemFromList(ui->listService, ui->buttonServiceDelete);
}
void FahrtagWindow::on_checkService_clicked(bool checked)
{
    fahrtag->setBenoetigeService(checked);
}

void FahrtagWindow::on_tablePersonen_cellChanged(int row, int column)
{
    if (nehme) {
        nehme = false;
        // column 1: Name, 2: Beginn, 3: Ende, 4: Aufgabe
        /*
     * Hier muss etwas aufwendiger gearbeitet werden,
     * denn die veränderte Spalte muss eingefügt werden
     * und die Liste muss up-todate gehalten werden
     * */

        // wenn name geändert wurde, muss der Index über die namen neu aufgebaut werden, da es sonst probleme gibt
        if (column == 0) {
            QStringList *neu = new QStringList();
            for(int i = 1; i <= ui->tablePersonen->rowCount(); i++) {
                QString n = "";
                if (ui->tablePersonen->item(i, 0) != nullptr)
                    n = ui->tablePersonen->item(i, 0)->text();
                neu->append(n);
                if (namen->contains(n)) {
                    namen->remove(n);
                }
            }
            if (namen->size() == 1) {
                fahrtag->removePerson(namen->values().at(0));
            }
        }
        QString name = "";
        if (ui->tablePersonen->item(row,0) != nullptr)
            name = ui->tablePersonen->item(row,0)->text();

        QString beginn = "";
        QTime beginnZ = QTime(0,0);
        if (ui->tablePersonen->item(row,1) != nullptr) {
            beginn = ui->tablePersonen->item(row,1)->text();
            if (beginn != "") {
                beginnZ = QTime::fromString(beginn, "h:mm");
                ui->tablePersonen->item(row,1)->setText(beginnZ.toString("hh:mm"));
            }
        }

        QString ende = "";
        QTime endeZ = QTime(0,0);
        if (ui->tablePersonen->item(row,2) != nullptr) {
            ende = ui->tablePersonen->item(row,2)->text();
            if (ende != "") {
                endeZ = QTime::fromString(ende, "h:mm");
                ui->tablePersonen->item(row,2)->setText(endeZ.toString("hh:mm"));
            }
        }

        QString aufgabe = "";
        if (ui->tablePersonen->item(row,3) != nullptr)
            aufgabe = ui->tablePersonen->item(row,3)->text();

        if (name.toUpper().contains("EXTERN")) {
            Person *p = new Person(name);
            p->setAusbildungTf(true);
            p->setAusbildungZf(true);
            p->setAusbildungRangierer(true);
            fahrtag->addPerson(p, "", beginnZ, endeZ, aufgabe);
        } else {
            ManagerPersonal::Misstake antw = fahrtag->addPerson(name, "", beginnZ, endeZ, aufgabe);

            switch (antw) {
            case ManagerPersonal::OK:
                break;
            case ManagerPersonal::PersonNichtGefunden:
                QMessageBox::warning(this, "Fehler", "Die eingegebene Person konnte im System nicht gefunden werden.");
                break;
            case ManagerPersonal::FalscheQualifikation:
                QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
                break;
            default:
                QMessageBox::warning(this, "Sonstiger Fehler", "Während der Verarbeitung der Eingabe ist ein Fehler unterlaufen.\nPrüfen Sie Ihre Eingaben und versuchen es erneut!");
                break;
            }
        }
        nehme = true;
    }
}

void FahrtagWindow::on_buttonInsert_clicked()
{
    // Neue Zeile einfügen
    ui->tablePersonen->insertRow(0);
    ui->buttonRemove->setEnabled(true);
}

void FahrtagWindow::on_buttonRemove_clicked()
{
    // Prüfen, ob die Zeile wirklich gelöscht werden darf, ansonsten einfach nicht löschen, bzw HInweis in entsprechender Liste löschen
    int i = ui->tablePersonen->currentRow();
    if (i == -1) return;
    QString n = "";
    if (ui->tablePersonen->item(i, 0) != nullptr) {
        if (ui->tablePersonen->item(i, 0)->flags() == Qt::NoItemFlags) {
            QMessageBox::information(this, "Nicht löschbar", "Diese Person müssen Sie in der ensprechenden Liste löschen!");
            return;
        }
        n = ui->tablePersonen->item(i, 0)->text();
    }
    if (fahrtag->removePerson(n)) {
        namen->remove(n);
    }
    ui->tablePersonen->removeRow(i);
    ui->buttonRemove->setEnabled(ui->tablePersonen->rowCount() > 0);
}

void FahrtagWindow::on_actionDelete_triggered()
{

}

void FahrtagWindow::on_actionPrint_triggered()
{
    QPrinter *p = Export::getPrinterPaper(this);
    Export::printFahrtag(fahrtag, nullptr, p);
}

void FahrtagWindow::on_actionPdf_triggered()
{
    QPrinter *p = Export::getPrinterPDF(this, windowTitle()+".pdf");
    Export::printFahrtag(fahrtag, p);
}

void FahrtagWindow::loadReservierung(Reservierung *r)
{
    nehmeRes = false;
    aktuelleRes = r;
    ui->lineName->setText(r->getName());
    ui->lineMail->setText(r->getMail());
    ui->lineTelefon->setText(r->getTelefon());
    ui->spinAnzahl->setValue(r->getAnzahl());
    ui->comboKlasse->setCurrentIndex(r->getKlasse());
    QList<QString> *z = r->getZuege();
    QList<QString> *h = r->getHps();
    ui->comboStart1Zug->setCurrentText(z->at(0));
    ui->comboEnde1Zug->setCurrentText(z->at(1));
    ui->comboStart1Hp->setCurrentText(h->at(0));
    ui->comboEnde1Hp->setCurrentText(h->at(1));
    ui->lineSitze->setText(Fahrtag::getStringFromPlaetze(r->getSitzplatz()));
    ui->lineSitze->setStyleSheet("background-color: #b9ceac");
    ui->checkFahrrad->setChecked(r->getFahrrad());
    ui->plainSonstiges->clear();
    ui->plainSonstiges->setPlainText(r->getSonstiges());
    nehmeRes = true;
}

void FahrtagWindow::saveResFahrt()
{
    QList<QString> *z = new QList<QString>();
    QList<QString> *h = new QList<QString>();
    z->append(ui->comboStart1Zug->currentText());
    z->append(ui->comboEnde1Zug->currentText());
    h->append(ui->comboStart1Hp->currentText());
    h->append(ui->comboEnde1Hp->currentText());
    aktuelleRes->setZuege(z);
    aktuelleRes->setHps(h);
}


void FahrtagWindow::on_buttonAdd_clicked()
{
    Reservierung *r = fahrtag->createReservierung();
    QListWidgetItem *i = new QListWidgetItem(r->getName());
    ui->listRes->insertItem(0, i);
    resToItem->insert(r, i);
    itemToRes->insert(i, r);
    ui->buttonDelete->setEnabled(true);
    loadReservierung(r);
}

void FahrtagWindow::on_buttonDelete_clicked()
{
    // Nachfrage ob wirklcih löschen
    if (QMessageBox::question(this, "Wirklich löschen?", "Möchten Sie die ausgwählte Reservierung unwiderruflich löschen?") == QMessageBox::Yes) {
        QListWidgetItem *i = ui->listRes->takeItem(ui->listRes->currentRow());
        Reservierung *r = itemToRes->value(i);
        fahrtag->removeReservierung(r);
        itemToRes->remove(i);
        resToItem->remove(r);
        if (ui->listRes->count() > 0) {
            loadReservierung(itemToRes->value(ui->listRes->item(0)));
        } else {
            nehmeRes = false;
            ui->buttonDelete->setEnabled(false);
        }
        update();
    }
}

void FahrtagWindow::on_listRes_itemDoubleClicked(QListWidgetItem *item)
{
    loadReservierung(itemToRes->value(item));
}

void FahrtagWindow::on_buttonShow_clicked()
{
    QMessageBox::information(this, "Ohne Funktion", QString::number(fahrtag->getAnzahl()));
}

void FahrtagWindow::on_buttonVerteile_clicked()
{
    // Informationen, wie lange es gedauert hat
    QTime start = QTime::currentTime();
    bool ok = fahrtag->verteileSitzplaetze();
    QTime ende = QTime::currentTime();
    if (nehme) {
        if (ok) {
            QMessageBox::information(this, tr("Fertig"), tr("Die Sitzplätze wurden in ")+QString::number(start.msecsTo(ende))+tr(" mSekunden verteilt."));
        } else {
            QMessageBox::information(this, tr("Fehler"), tr("Es konnten nicht alle Reservierungen verteilt werden!\nBitte prüfen Sie die Verteilung auf mögliche Fehler!"));
        }
        fahrtag->emitter();
    } else {
        qDebug() << start.msecsTo(ende);
    }
}

void FahrtagWindow::on_checkBoxAuto_clicked(bool checked)
{
    if (nehme) {
        fahrtag->setAutoPlatz(checked);
        fahrtag->emitter();
    }
    ui->checkBoxAll->setEnabled(checked);
    ui->buttonVerteile->setEnabled(checked);
}

void FahrtagWindow::on_lineName_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setName(arg1);
        resToItem->value(aktuelleRes)->setText(aktuelleRes->getName());
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_lineMail_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setMail(arg1);
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_lineTelefon_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setTelefon(arg1);
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_spinAnzahl_valueChanged(int arg1)
{
    if (nehmeRes) {
        aktuelleRes->setAnzahl(arg1);
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_comboKlasse_currentIndexChanged(int index)
{
    if (nehmeRes) {
        aktuelleRes->setKlasse(index);
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_comboStart1Zug_currentTextChanged(const QString &arg1)
{
    if (nehmeRes)
        saveResFahrt();
}

void FahrtagWindow::on_comboStart1Hp_currentTextChanged(const QString &arg1)
{
    if (nehmeRes)
        saveResFahrt();
}

void FahrtagWindow::on_comboEnde1Zug_currentTextChanged(const QString &arg1)
{
    if (nehmeRes)
        saveResFahrt();
}

void FahrtagWindow::on_comboEnde1Hp_currentTextChanged(const QString &arg1)
{
    if (nehmeRes)
        saveResFahrt();
}

void FahrtagWindow::on_lineSitze_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        QMap<int, QList<int>*> *plaetze = ManagerReservierungen::getPlaetzeFromString(arg1);
        bool ok = fahrtag->checkPlaetze(plaetze);

        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        // Speichere die Sitzplätze, wenn die valide sind
        if (ok) {
            aktuelleRes->setSitzplatz(plaetze);
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
            update();
        } else {
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
        }
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_lineSitze_returnPressed()
{
    // prüfe, ob die sitzplätze valide sidn und speichere sie
    if (nehmeRes) {
        QMap<int, QList<int>*> *plaetze = ManagerReservierungen::getPlaetzeFromString(ui->lineSitze->text());
        bool ok = fahrtag->checkPlaetze(plaetze);

        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        // Speichere die Sitzplätze, wenn die valide sind
        if (ok) {
            aktuelleRes->setSitzplatz(plaetze);
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
            update();
        } else {
            QMessageBox::information(this, "Sitzplätze fehlerhaft", "Die eingegebenen Sitzplätze konnten nicht zugewiesen werden.\nGeben Sie gültige und freie Sitzplätze ein!");
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
        }
    }
}

void FahrtagWindow::on_checkFahrrad_clicked(bool checked)
{
    if (nehmeRes) {
        aktuelleRes->setFahrrad(checked);
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_plainSonstiges_textChanged()
{
    if (nehmeRes) {
        aktuelleRes->setSonstiges(ui->plainSonstiges->toPlainText());
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_listRes_itemClicked(QListWidgetItem *item)
{
    loadReservierung(itemToRes->value(item));
}

void FahrtagWindow::on_checkBoxBenoetigt_clicked(bool checked)
{
    if (nehme) {
        fahrtag->setPersonalBenoetigt(checked);
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_checkBoxAll_clicked(bool checked)
{
    if (nehme){
        if (checked) QMessageBox::information(this, tr("Hinweis"), tr("Es kann unter Umständen sehr lange dauern, bis die 'perfekte' Verteilung berechnet wird.\nIhr Computer reagiert in dieser Zeit vielleicht nicht!\nDiese Funktion sollten Sie nur für eine kleine Anzahl an Reservierungen benutzen."));
        fahrtag->setCheckAll(checked);
        fahrtag->emitter();
    }
}
