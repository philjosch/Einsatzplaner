#include "fahrtagwindow.h"
#include "person.h"
#include "ui_fahrtagwindow.h"
#include <QMessageBox>
#include <QListWidget>

FahrtagWindow::FahrtagWindow(QWidget *parent, Fahrtag *f) :
    QMainWindow(parent),
    ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
    ui->frameReservierungen = f;
    fahrtag = f;
    loadData();
    ui->dateDate->setFocus();
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum()->toString("dddd dd. MM. yyyy"));

    nehme = true;
    listeMitNamen = new QMap<QListWidgetItem *, QString>();

    listToTable = new QMap<QListWidgetItem*,QTableWidgetItem*>();

    namen = new QSet<QString>();

}

FahrtagWindow::~FahrtagWindow()
{
    delete ui;
}

void FahrtagWindow::on_dateDate_dateChanged(const QDate &date)
{
    fahrtag->setDatum(new QDate(date.year(), date.month(), date.day()));
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum()->toString("dddd dd. MM. yyyy"));
}

void FahrtagWindow::on_comboArt_currentIndexChanged(int index)
{
    fahrtag->setArt((Fahrtag::Art)index);
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum()->toString("dddd dd. MM. yyyy"));
}

void FahrtagWindow::on_textAnlass_textChanged()
{
    fahrtag->setAnlass(ui->textAnlass->toPlainText());
}

void FahrtagWindow::on_checkWichtig_stateChanged(int arg1)
{
    fahrtag->setWichtig(arg1 == 1);
}

void FahrtagWindow::on_comboWagenreihung_currentTextChanged(const QString &arg1)
{
    fahrtag->setWagenreihung(arg1);
}

void FahrtagWindow::on_comboTimeTfH_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitTf(new QTime(arg1.toInt(), fahrtag->getZeitTf()->minute()));
}

void FahrtagWindow::on_comboTimeTfM_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitTf(new QTime(fahrtag->getZeitTf()->hour(), arg1.toInt()));
}

void FahrtagWindow::on_comboTimeZH_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitAnfang(new QTime(arg1.toInt(), fahrtag->getZeitAnfang()->minute()));
}

void FahrtagWindow::on_comboTimeZM_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitAnfang(new QTime(fahrtag->getZeitAnfang()->hour(), arg1.toInt()));
}

void FahrtagWindow::on_textBemerkungen_textChanged()
{
    fahrtag->setBemerkungen(ui->textBemerkungen->toPlainText());
}

void FahrtagWindow::on_plainBeschreibung_textChanged()
{
// Dekativiert, wird im Moment nicht benoetigt
}

void FahrtagWindow::on_timeBeginn_timeChanged(const QTime &time)
{
    fahrtag->setZeitAnfang(new QTime(time.hour(), time.minute()));
}

void FahrtagWindow::on_timeEnde_timeChanged(const QTime &time)
{
    fahrtag->setZeitEnde(new QTime(time.hour(), time.minute()));
}

void FahrtagWindow::on_checkBoxBenoetigt_stateChanged(int arg1)
{
    fahrtag->setPersonalBenoetigt(arg1 == 1);
}


void FahrtagWindow::loadData()
{
    // Allgemeine Daten von AActivity
    ui->dateDate->setDate(*fahrtag->getDatum());
    ui->textAnlass->clear();
    ui->textAnlass->insertPlainText(fahrtag->getAnlass());
    ui->timeEnde->setTime(* fahrtag->getZeitEnde());
    ui->checkBoxBenoetigt->setChecked(fahrtag->getPersonalBenoetigt());

    // Daten von ManagerReservierungen
    ui->comboWagenreihung->setCurrentText(fahrtag->getWagenreihung());

    // Daten von Fahrtag
    /* LIstTf, ListZf, ListZub, ListService fehlt noch */
    ui->comboArt->setCurrentIndex((int)fahrtag->getArt());
    ui->comboTimeTfH->setCurrentText(fahrtag->getZeitTf()->toString("HH"));
    ui->comboTimeTfM->setCurrentText(fahrtag->getZeitTf()->toString("mm"));
    ui->comboTimeZH->setCurrentText(fahrtag->getZeitAnfang()->toString("HH"));
    ui->comboTimeZM->setCurrentText(fahrtag->getZeitAnfang()->toString("mm"));
    ui->checkWichtig->setChecked(fahrtag->getWichtig());
    ui->checkTf->setChecked(fahrtag->getBenoetigeTf());
    ui->checkZf->setChecked(fahrtag->getBenoetigeZf());
    ui->checkZub->setChecked(fahrtag->getBenoetigeZub());
    ui->checkService->setChecked(fahrtag->getBenoetigeService());

}


void FahrtagWindow::addItemTolist(QListWidget *l, QPushButton *b)
{
    l->addItem("(Name eingeben)");
    l->item(l->count()-1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
    b->setEnabled(true);
}
void FahrtagWindow::deleteItemFromList(QListWidget *l, QPushButton *b)
{
    l->takeItem(l->currentRow());
    b->setEnabled(l->count()>0);
}

void FahrtagWindow::on_listTf_itemChanged(QListWidgetItem *item)
{
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

    QString nameAlt = listeMitNamen->value(item);
    listeMitNamen->insert(item, name);

    if (bem.contains("Azubi") || bem.contains("FS") || bem.contains("Extern")) {
        fahrtag->addTf(name, bem);
        if (listToTable->contains(item))
            listToTable->remove(item);
        return;
    } else if (!fahrtag->getPersonal()->personExists(name)) {
        QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
        listeMitNamen->insert(item, nameAlt);
        return;
    }

    Person *person = fahrtag->getPersonal()->getPerson(name);
    name = person->getName();

    // Prüfen, ob die Person die Qualifikationen hat
    if (! person->getAusbildungTf()) {
        QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
        listeMitNamen->insert(item, nameAlt);
        return;
    }

    person->addActivity(fahrtag, kat);

    // Wenn die Person in der Tabelle ist, muss dort der aneme bei bedarf geändert werden
    // Außerdem muss die Person zum Fahrtag hinzugefügt werden und die alte gelsöcht werden
    if (listToTable->contains(item)) {
        if (nameAlt == name) {
            ui->tablePersonen->item(ui->tablePersonen->row(listToTable->value(item)), 3)->setText(AActivity::getStringFromCategory(kat));
            return;
            //Nichts zu machen, da der Name nicht verändert wurde
        } else {
            fahrtag->removeTf(nameAlt);
            fahrtag->getPersonal()->getPerson(nameAlt)->removeActivity(fahrtag);
            ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
            namen->remove(nameAlt);
            listToTable->remove(item);
        }
    }
    fahrtag->addTf(name, bem);
    if (! listToTable->contains(item)) {
        QTableWidgetItem *t = new QTableWidgetItem(name);
        t->setFlags(Qt::NoItemFlags);
        QTableWidgetItem *t2 = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
        t2->setFlags(Qt::NoItemFlags);
        ui->tablePersonen->insertRow(0);
        ui->tablePersonen->setItem(0,0,t);
        ui->tablePersonen->setItem(0,3,t2);
        listToTable->insert(item, t);
        namen->insert(name);
    }
    // alle daten in eine Liste laden und dann
    // Abgelich, welcer Namen geändert wurde
    // finden dieser Zeile in der Tabelle und abändern des namens
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
    } else if (!fahrtag->getPersonal()->personExists(name)) {
    } else {

        Person *person = fahrtag->getPersonal()->getPerson(name);
        name = person->getName();
        person->removeActivity(fahrtag);
        fahrtag->removeTf(name);
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

void FahrtagWindow::on_listZf_itemChanged(QListWidgetItem *item)
{
    QString text = item->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }
    AActivity::Category kat = AActivity::Zf;

    QString nameAlt = listeMitNamen->value(item);
    listeMitNamen->insert(item, name);

    if (bem.contains("Extern")) {
        fahrtag->addZf(name, bem);
        if (listToTable->contains(item))
            listToTable->remove(item);
        return;
    } else if (!fahrtag->getPersonal()->personExists(name)) {
        QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
        listeMitNamen->insert(item, nameAlt);
        return;
    }

    Person *person = fahrtag->getPersonal()->getPerson(name);
    name = person->getName();

    // Prüfen, ob die Person die Qualifikationen hat
    if (! person->getAusbildungZf()) {
        QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
        listeMitNamen->insert(item, nameAlt);
        return;
    }

    person->addActivity(fahrtag, kat);

    // Wenn die Person in der Tabelle ist, muss dort der aneme bei bedarf geändert werden
    // Außerdem muss die Person zum Fahrtag hinzugefügt werden und die alte gelsöcht werden
    if (listToTable->contains(item)) {
        if (nameAlt == name) {
            ui->tablePersonen->item(ui->tablePersonen->row(listToTable->value(item)), 3)->setText(AActivity::getStringFromCategory(kat));
            return;
            //Nichts zu machen, da der Name nicht verändert wurde
        } else {
            fahrtag->removeZf(nameAlt);
            fahrtag->getPersonal()->getPerson(nameAlt)->removeActivity(fahrtag);
            ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
            namen->remove(nameAlt);
            listToTable->remove(item);
        }
    }
    fahrtag->addZf(name, bem);
    if (! listToTable->contains(item)) {
        QTableWidgetItem *t = new QTableWidgetItem(name);
        t->setFlags(Qt::NoItemFlags);
        QTableWidgetItem *t2 = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
        t2->setFlags(Qt::NoItemFlags);
        ui->tablePersonen->insertRow(0);
        ui->tablePersonen->setItem(0,0,t);
        ui->tablePersonen->setItem(0,3,t2);
        listToTable->insert(item, t);
        namen->insert(name);
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
        fahrtag->removeZf(name);
    }
    QListWidgetItem *item = ui->listZf->currentItem();
    if (listeMitNamen->contains(item)) {
        listeMitNamen->remove(item);
    }
    if (listToTable->contains(item)) {
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
        listToTable->remove(item);
    }
    deleteItemFromList(ui->listTf, ui->buttonTfDelete);
    deleteItemFromList(ui->listZf, ui->buttonZfDelete);
}

void FahrtagWindow::on_listZub_itemChanged(QListWidgetItem *item)
{
    QString text = item->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }
    AActivity::Category kat = AActivity::Begleiter;

    QString nameAlt = listeMitNamen->value(item);
    listeMitNamen->insert(item, name);

    if (bem.contains("Extern")) {
        fahrtag->addZub(name, bem);
        if (listToTable->contains(item))
            listToTable->remove(item);
        return;
    } else if (!fahrtag->getPersonal()->personExists(name)) {
        QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
        listeMitNamen->insert(item, nameAlt);
        return;
    }

    Person *person = fahrtag->getPersonal()->getPerson(name);
    name = person->getName();

    // Prüfen, ob die Person die Qualifikationen hat
/*    if (! person->getAusbildungTf()) {
        QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
        listeMitNamen->insert(item, nameAlt);
        return;
    }
*/
    person->addActivity(fahrtag, kat);

    // Wenn die Person in der Tabelle ist, muss dort der aneme bei bedarf geändert werden
    // Außerdem muss die Person zum Fahrtag hinzugefügt werden und die alte gelsöcht werden
    if (listToTable->contains(item)) {
        if (nameAlt == name) {
            ui->tablePersonen->item(ui->tablePersonen->row(listToTable->value(item)), 3)->setText(AActivity::getStringFromCategory(kat));
            return;
            //Nichts zu machen, da der Name nicht verändert wurde
        } else {
            fahrtag->removeZub(nameAlt);
            fahrtag->getPersonal()->getPerson(nameAlt)->removeActivity(fahrtag);
            ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
            namen->remove(nameAlt);
            listToTable->remove(item);
        }
    }
    fahrtag->addZub(name, bem);
    if (! listToTable->contains(item)) {
        QTableWidgetItem *t = new QTableWidgetItem(name);
        t->setFlags(Qt::NoItemFlags);
        QTableWidgetItem *t2 = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
        t2->setFlags(Qt::NoItemFlags);
        ui->tablePersonen->insertRow(0);
        ui->tablePersonen->setItem(0,0,t);
        ui->tablePersonen->setItem(0,3,t2);
        listToTable->insert(item, t);
        namen->insert(name);
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
        fahrtag->removeZub(name);
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

void FahrtagWindow::on_listService_itemChanged(QListWidgetItem *item)
{
    QString text = item->text();
    QStringList liste = text.split("; ");
    QString name = liste.at(0);
    QString bem = "";
    if (liste.length() > 1) {
        bem = liste.at(1);
    }
    AActivity::Category kat = AActivity::Service;

    QString nameAlt = listeMitNamen->value(item);
    listeMitNamen->insert(item, name);

    if (bem.contains("Extern")) {
        fahrtag->addService(name, bem);
        if (listToTable->contains(item))
            listToTable->remove(item);
        return;
    } else if (!fahrtag->getPersonal()->personExists(name)) {
        QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
        listeMitNamen->insert(item, nameAlt);
        return;
    }

    Person *person = fahrtag->getPersonal()->getPerson(name);
    name = person->getName();

    person->addActivity(fahrtag, kat);

    // Wenn die Person in der Tabelle ist, muss dort der aneme bei bedarf geändert werden
    // Außerdem muss die Person zum Fahrtag hinzugefügt werden und die alte gelsöcht werden
    if (listToTable->contains(item)) {
        if (nameAlt == name) {
            ui->tablePersonen->item(ui->tablePersonen->row(listToTable->value(item)), 3)->setText(AActivity::getStringFromCategory(kat));
            return;
            //Nichts zu machen, da der Name nicht verändert wurde
        } else {
            fahrtag->removeService(nameAlt);
            fahrtag->getPersonal()->getPerson(nameAlt)->removeActivity(fahrtag);
            ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable->value(item)));
            namen->remove(nameAlt);
            listToTable->remove(item);
        }
    }
    fahrtag->addService(name, bem);
    if (! listToTable->contains(item)) {
        QTableWidgetItem *t = new QTableWidgetItem(name);
        t->setFlags(Qt::NoItemFlags);
        QTableWidgetItem *t2 = new QTableWidgetItem(AActivity::getStringFromCategory(kat));
        t2->setFlags(Qt::NoItemFlags);
        ui->tablePersonen->insertRow(0);
        ui->tablePersonen->setItem(0,0,t);
        ui->tablePersonen->setItem(0,3,t2);
        listToTable->insert(item, t);
        namen->insert(name);
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
        fahrtag->removeService(name);
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


void FahrtagWindow::on_tablePersonen_cellChanged(int row, int column)
{
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
        beginnZ = QTime::fromString(beginn, "h:mm");
        ui->tablePersonen->item(row,1)->setText(beginnZ.toString("hh:mm"));
    }

    QString ende = "";
    QTime endeZ = QTime(0,0);
    if (ui->tablePersonen->item(row,2) != nullptr) {
        ende = ui->tablePersonen->item(row,2)->text();
        endeZ = QTime::fromString(ende, "h:mm");
        ui->tablePersonen->item(row,2)->setText(endeZ.toString("hh:mm"));
    }

    QString aufgabe = "";
    if (ui->tablePersonen->item(row,3) != nullptr)
        aufgabe = ui->tablePersonen->item(row,3)->text();

    ManagerPersonal::Misstake antw = fahrtag->addPerson(name, aufgabe, &beginnZ, &endeZ);

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
    if (ui->tablePersonen->item(i, 0)->flags() == Qt::NoItemFlags) {
        QMessageBox::information(this, "Nicht löschbar", "Diese Person müssen Sie in der ensprechenden Liste löschen!");
        return;
    }
    QString n = ui->tablePersonen->item(i, 0)->text();
    if (fahrtag->removePerson(n)) {
        namen->remove(n);
        ui->tablePersonen->removeRow(ui->tablePersonen->currentRow());
        ui->buttonRemove->setEnabled(ui->tablePersonen->rowCount() > 0);
    }
}
