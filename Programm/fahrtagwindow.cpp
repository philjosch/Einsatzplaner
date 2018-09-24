#include "fahrtagwindow.h"
#include "person.h"
#include "ui_fahrtagwindow.h"
#include "export.h"
#include "coreapplication.h"

#include <QMessageBox>

FahrtagWindow::FahrtagWindow(QWidget *parent, Fahrtag *f) : QMainWindow(parent), ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
//    ui->lineSitze->setValidator(new QRegExpValidator(QRegExp("2[0-9]{2}\\s*\\:\\s*(((([1-9][0-9]?)\\s*-\\s*([1-9][0-9]?))|([1-9][0-9]?)))(\\s*,\\s*((([1-9][0-9]?)-([1-9][0-9]?))|([1-9][0-9]?)))*(;\\s*[1-9][0-9]?\\s*\\:\\s*(((([1-9][0-9]?)\\s*-\\s*([1-9][0-9]?))|([1-9][0-9]?)))(\\s*,\\s*((([1-9][0-9]?)-([1-9][0-9]?))|([1-9][0-9]?)))*\\s*)*")));

    fahrtag = f;
    nehme = true;
    ui->dateDate->setFocus();
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum().toString("dddd dd. MM. yyyy"));

    listeMitNamen = QMap<QListWidgetItem *, QString>();
    listToTable = QMap<QListWidgetItem*,QTableWidgetItem*>();
    namen = QSet<QString>();

    resToItem = QMap<Reservierung*, QListWidgetItem*>();
    itemToRes = QMap<QListWidgetItem*, Reservierung*>();

    nehmeRes = false;
    widgetInTableToTableWidget = QMap<QWidget*, QTableWidgetItem*>();

    loadData();
    update();
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
    Fahrtag::Art art = static_cast<Fahrtag::Art>(index);
    if (nehme) {
        fahrtag->setArt(art);
    }
    ui->tabReservierungen->setEnabled(art != Fahrtag::Schnupperkurs);
    ui->checkZf->setEnabled(art != Fahrtag::Schnupperkurs);
    ui->checkZub->setEnabled(art != Fahrtag::Schnupperkurs);
    ui->checkService->setEnabled(art != Fahrtag::Schnupperkurs);

    ui->checkBoxAll->setEnabled(Fahrtag::Nikolauszug == art);
    ui->buttonVerteile->setEnabled(Fahrtag::Nikolauszug == art);

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
        fahrtag->setWichtig(arg1 == 2);
}

void FahrtagWindow::on_comboWagenreihung_currentTextChanged(const QString &arg1)
{
    if (nehme) {
        QString alt = fahrtag->getWagenreihung();
        if (fahrtag->setWagenreihung(arg1)) {
            update();
        } else {
            QMessageBox::information(this, tr("Fehler"), tr("Die Wagenreihung konnte nicht geändert werden. Da es Reservierungen in den weggefallenen Waggons gibt!"));
            nehme = false;
            ui->comboWagenreihung->setCurrentText(alt);
            nehme = true;
        }
    }
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


void FahrtagWindow::on_comboTimeEndeH_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setZeitEnde(QTime(arg1.toInt(), fahrtag->getZeitEnde().minute()));
}

void FahrtagWindow::on_comboTimeEndeM_currentTextChanged(const QString &arg1)
{
    if (nehme)
        fahrtag->setZeitEnde(QTime(fahrtag->getZeitEnde().hour(), arg1.toInt()));
}

void FahrtagWindow::loadData()
{
    if (nehme) {
        nehme = false;
        // Allgemeine Daten von AActivity
        ui->dateDate->setDate(fahrtag->getDatum());
        ui->textAnlass->clear();
        ui->textAnlass->insertPlainText(fahrtag->getAnlass());
        ui->comboTimeEndeH->setCurrentText(fahrtag->getZeitEnde().toString("HH"));
        ui->comboTimeEndeM->setCurrentText(fahrtag->getZeitEnde().toString("mm"));
        ui->checkBoxBenoetigt->setChecked(fahrtag->getPersonalBenoetigt());
        ui->textBemerkungen->clear();
        ui->textBemerkungen->insertPlainText(fahrtag->getBemerkungen());

        // Daten von Manager_Reservierungen
        ui->comboWagenreihung->setCurrentText(fahrtag->getWagenreihung());
        QSetIterator<Reservierung*> iter = fahrtag->getReservierungen();
        while(iter.hasNext()) {
            Reservierung *r = iter.next();
            QListWidgetItem *item = new QListWidgetItem(r->getName());
            resToItem.insert(r, item);
            itemToRes.insert(item, r);
            ui->listRes->insertItem(0, item);
            ui->buttonDelete->setEnabled(true);
        }
        ui->listRes->sortItems();
        ui->checkBoxAll->setEnabled(false);
        ui->buttonVerteile->setEnabled(Fahrtag::Nikolauszug == fahrtag->getArt());

        ui->checkBoxAll->setChecked(fahrtag->getCheckAll());
        ui->checkBoxAll->setEnabled(fahrtag->getArt() == Fahrtag::Nikolauszug);
        ui->buttonVerteile->setEnabled(fahrtag->getArt() == Fahrtag::Nikolauszug);

        // Daten von Fahrtag
        ui->comboArt->setCurrentIndex(fahrtag->getArt());
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

        listeMitNamen = QMap<QListWidgetItem*, QString>();
        listToTable = QMap<QListWidgetItem*, QTableWidgetItem*>();
        namen = QSet<QString>();

        // Fügt die Personen in das Fahrtagfesnter ein
        for(Person *p: fahrtag->getPersonen().keys()) {
            AActivity::Infos *info = fahrtag->getPersonen().value(p);

            // Fügt die Personene in die einzelnen Listen ein, sodass es direkt geändert werden kann
            QListWidgetItem *item = new QListWidgetItem(p->getName() + (info->bemerkung == "" ? "" : "; "+info->bemerkung));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
            listeMitNamen.insert(item, p->getName());
            bool block = true;
            switch (fahrtag->getPersonen().value(p)->kategorie) {
                case Category::Tf:
                case Category::Tb:
                    tf.insert(p, info);
                    ui->listTf->insertItem(0, item);
                    ui->buttonTfDelete->setEnabled(true);
                    break;
                case Category::Zf:
                    zf.insert(p, info);
                    ui->listZf->insertItem(0, item);
                    ui->buttonZfDelete->setEnabled(true);
                    break;
                case Category::Zub:
                case Category::Begleiter:
                    zub.insert(p, info);
                    ui->listZub->insertItem(0, item);
                    ui->buttonZubDelete->setEnabled(true);
                    break;
                case Category::Service:
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

            if (! AActivity::isExtern(info->bemerkung)) {
                // Zeile für die Person in die Tabelle einfügen
                namen.insert(p->getName());
                on_buttonInsert_clicked();

                ui->tablePersonen->item(0, 0)->setText(p->getName());
                if (block) ui->tablePersonen->item(0, 0)->setFlags(Qt::NoItemFlags);
                Category kat = info->kategorie;
                if (kat == Category::Begleiter) kat = Category::Zub;
                static_cast<QComboBox*>(ui->tablePersonen->cellWidget(0, 1))->setCurrentText(AActivity::getStringFromCategory(kat));
                static_cast<QComboBox*>(ui->tablePersonen->cellWidget(0, 1))->setEnabled(!block);
                static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(0, 2))->setTime(info->beginn);
                static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(0, 3))->setTime(info->ende);
                QTableWidgetItem *zelleBemerkung = new QTableWidgetItem(info->bemerkung);
                if(block) zelleBemerkung->setFlags(Qt::NoItemFlags);
                ui->tablePersonen->setItem(0, 4, zelleBemerkung);

                listToTable.insert(item, ui->tablePersonen->item(0,0));
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
    if (l->currentItem() == nullptr) return;
    QListWidgetItem *item = l->currentItem();
    QString text = item->text();
    QStringList liste = text.split(QRegExp("\\s*;\\s*"));
    QString name = liste.at(0);

    fahrtag->removePerson(name);
    if (fahrtag->getPersonal()->personExists(name)) {
        Person *person = fahrtag->getPersonal()->getPerson(name);
        person->removeActivity(fahrtag);
    }

    if (listeMitNamen.contains(item)) {
        listeMitNamen.remove(item);
    }
    if (listToTable.contains(item)) {
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable.value(item)));
        listToTable.remove(item);
    }

    l->takeItem(l->currentRow());
    b->setEnabled(l->count()>0);
}

void FahrtagWindow::itemChanged(QListWidgetItem *item , Category kat)
{
    /* item: Das Item das Verändert wurde */
    /* kat: Die Kategorie, auf die gebucht wird */
    QString text = item->text();
    QStringList liste = text.split(QRegExp("\\s*;\\s*"));
    QString name = liste.at(0);
    liste.removeFirst();
    QString bem = "";
    while (liste.length() > 0) {
        bem += liste.first();
        if (liste.length() > 1) bem += "; ";
        liste.removeFirst();
    }

    if (listeMitNamen.contains(item)) {
        QString nameAlt = listeMitNamen.value(item);
        if (name == nameAlt) {
            // Bemerkung aktualisieren und speichern
            int row = ui->tablePersonen->row((listToTable.value(item)));
            QTime beginn = dynamic_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 2))->time();
            QTime ende = dynamic_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 3))->time();
            Mistake antw = fahrtag->addPerson(name, bem, beginn, ende, kat);
            switch (antw) {
            case OK:
                ui->tablePersonen->item(row, 4)->setText(bem);
                break;
            default:
                QMessageBox::warning(this, "Fehler", "Bei der Eintragung der Person ist ein Fehler aufgetreten, bitte überprüfen Sie Ihre Eingaben. Die Person wurde nicht gespeichert!");
            }
            return;
        } else {
            // Eintrag der alten person löschen
            fahrtag->removePerson(nameAlt);
            namen.remove(nameAlt);
            listeMitNamen.remove(item);
            ui->tablePersonen->removeRow(ui->tablePersonen->row(listToTable.value(item)));
            listToTable.remove(item);
        }
    }

    QTime beginn = QTime(0,0);
    QTime ende = QTime(0,0);
    Mistake antw = fahrtag->addPerson(name, bem, beginn, ende, kat);
    switch (antw) {
    case Mistake::ExternOk:
        break;
        // Verfahren, ähnlich wie bei einer normalen person, außer dass die Person nicht in die Tabelle eingefügt wird
    case Mistake::OK:
        // Neue Person in Ordnung:
        namen.insert(name);
        listeMitNamen.insert(item, name);

        // Zeile für die Person in die Tabelle einfügen
        on_buttonInsert_clicked();
        // Name
        ui->tablePersonen->item(0, 0)->setText(name);
        ui->tablePersonen->item(0, 0)->setFlags(Qt::NoItemFlags);
        // Aufgabe
        (dynamic_cast<QComboBox*>(ui->tablePersonen->cellWidget(0, 1)))->setCurrentText(AActivity::getStringFromCategory(kat));
        ui->tablePersonen->cellWidget(0, 1)->setDisabled(true);
        // Bemerkung
        ui->tablePersonen->item(0, 4)->setText(bem);
        ui->tablePersonen->item(0, 4)->setFlags(Qt::NoItemFlags);

        listToTable.insert(item, ui->tablePersonen->item(0, 0));
        break;

    case Mistake::FalscheQualifikation:
        QMessageBox::warning(this, "Fehlende Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
        break;
    case Mistake::PersonNichtGefunden:
        QMessageBox::information(this, "Person nicht gefunden", "Die eingegebene Person konnte nicht gefunden werden!");
        break;
    default:
        QMessageBox::information(this, "Fehler", "Beim Hinzufügen der Person zum Fahrtag ist ein Fehler aufgetreten! Bitte überprüfen Sie ihre Eingaben!");
        break;
    }
}

void FahrtagWindow::plausibilityCheck()
{
    int z1 = ui->comboStart1Zug->currentIndex();
    int z2 = ui->comboEnde1Zug->currentIndex();
    int z3 = ui->comboStart2Zug->currentIndex();
    int z4 = ui->comboEnde2Zug->currentIndex();

    int h1 = ui->comboStart1Hp->currentIndex();
    int h2 = ui->comboEnde1Hp->currentIndex();
    int h3 = ui->comboStart2Hp->currentIndex();
    int h4 = ui->comboEnde2Hp->currentIndex();

    bool z_ok = ( z1 <= z2 || z1 == 6 || z2 == 6) && (z2 < z3 || z2 == 6 || z3 == 6) && (z3 <= z4 || z3 == 6 || z4 == 6);
    bool h_ok1 = (h1 != h2 || h1 == 11 || h2 == 11 || z1 < z2)
            && (z1 != z2 || z1 == 6 || z2 == 6 || h1 == 11 || h2 == 11 || ((z1 %2 != 0 || h1 < h2) && (z1%2 != 1 || h1 > h2)))
            && (h1 == 11 || z1 == 6 || ((z1%2 != 0 || h1 != 10) && (z1%2 != 1 || h1 !=  0)))
            && (h2 == 11 || z2 == 6 || ((z2%2 != 0 || h2 !=  0) && (z2%2 != 1 || h2 != 10)));
    bool h_ok2 = (h3 != h4 || h3 == 11 || h4 == 11 || z3 < z4)
            && (z3 != z4 || z3 == 6 || z4 == 6 || h3 == 11 || h4 == 11 || ((z3 %2 != 0 || h3 < h4) && (z3%2 != 1 || h3 > h4)))
            && (h3 == 11 || z3 == 6 || ((z3%2 != 0 || h3 != 10) && (z3%2 != 1 || h3 !=  0)))
            && (h4 == 11 || z4 == 6 || ((z4%2 != 0 || h4 !=  0) && (z4%2 != 1 || h4 != 10)));

    if (! (z_ok && h_ok1 && h_ok2)) {
        QMessageBox::warning(this, "Plausibilitätsprüfung", "Bitte überprüfen Sie ihre Eingaben bezüglich der Fahrstrecken, da das System eine mögliche Unstimmigkeit festgestllt hat. Ihre Daten werden dennoch gespeichert!");
    }
}

void FahrtagWindow::on_listTf_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        QStringList liste = item->text().split(QRegExp("\\s*;\\s*"));
        QString bem = "";
        if (liste.length() > 1) bem = liste.at(1).toUpper();
        itemChanged(item, Category::Tf);
        nehme = true;
    }
}
void FahrtagWindow::on_buttonTfAdd_clicked()
{
    addItemTolist(ui->listTf, ui->buttonTfDelete);
}
void FahrtagWindow::on_buttonTfDelete_clicked()
{
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
        itemChanged(item, Category::Zf);
        nehme = true;
    }
}
void FahrtagWindow::on_buttonZfAdd_clicked()
{
    addItemTolist(ui->listZf, ui->buttonZfDelete);
}
void FahrtagWindow::on_buttonZfDelete_clicked()
{
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
        itemChanged(item, Category::Zub);
        nehme = true;
    }
}
void FahrtagWindow::on_buttonZubAdd_clicked()
{
    addItemTolist(ui->listZub, ui->buttonZubDelete);
}
void FahrtagWindow::on_buttonZubDelete_clicked()
{
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
        itemChanged(item, Category::Service);
        nehme = true;
    }
}
void FahrtagWindow::on_buttonServiceAdd_clicked()
{
    addItemTolist(ui->listService, ui->buttonServiceDelete);
}
void FahrtagWindow::on_buttonServiceDelete_clicked()
{
    deleteItemFromList(ui->listService, ui->buttonServiceDelete);
}
void FahrtagWindow::on_checkService_clicked(bool checked)
{
    fahrtag->setBenoetigeService(checked);
}

void FahrtagWindow::on_tablePersonen_cellChanged(int row, int column)
{
    ui->tablePersonen->resizeColumnsToContents();
    if (nehme) {
        nehme = false;
        // column 0: Name, 1: Aufgabe, 2: Beginn, 3: Ende, 4: Bemerkung
        // wenn name geändert wurde, muss der Index über die namen neu aufgebaut werden, da es sonst probleme gibt
        if (column == 0) {
            QSet<QString> neu = QSet<QString>();
            for( int i = 0; i <= ui->tablePersonen->rowCount(); i++) {
                QString n = (ui->tablePersonen->item(i, 0) == nullptr) ? "" : ui->tablePersonen->item(i, 0)->text();
                neu.insert(n);
                if (namen.contains(n)) {
                    namen.remove(n);
                }
            }

            if (namen.size() == 1) fahrtag->removePerson(namen.values().at(0));
            namen = neu;
        }

        QString name = (ui->tablePersonen->item(row,0) == nullptr) ? "" : ui->tablePersonen->item(row,0)->text();
        Category kat = AActivity::getCategoryFromString(static_cast<QComboBox*>(ui->tablePersonen->cellWidget(row, 1))->currentText());
        QTime beginn = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 2))->time();
        QTime ende = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 3))->time();
        QString bemerkung = (ui->tablePersonen->item(row, 4) == nullptr) ? "" :  ui->tablePersonen->item(row,4)->text();

        switch (fahrtag->addPerson(name, bemerkung, beginn, ende, kat)) {
        case Mistake::OK:
        case Mistake::ExternOk:
            break;
        case Mistake::PersonNichtGefunden:
            QMessageBox::warning(this, "Fehler", "Die eingegebene Person konnte im System nicht gefunden werden.");
            break;
        case Mistake::FalscheQualifikation:
            QMessageBox::warning(this, "Fehlene Qualifikation", "Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt.");
            break;
        default:
            QMessageBox::warning(this, "Sonstiger Fehler", "Während der Verarbeitung der Eingabe ist ein Fehler unterlaufen.\nPrüfen Sie Ihre Eingaben und versuchen es erneut!");
            break;
        }
        nehme = true;
    }
}

void FahrtagWindow::on_buttonInsert_clicked()
{
    bool nehmeOld = nehme;
    nehme = false;
    ui->tablePersonen->insertRow(0);

    QTableWidgetItem *item = new QTableWidgetItem("");
    ui->tablePersonen->setItem(0, 0, item);

    QComboBox *box = fahrtag->generateNewCategoryComboBox();
    connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(complexWidgetInTableChanged()));
    ui->tablePersonen->setCellWidget(0, 1, box);

    QTimeEdit *beginn = fahrtag->generateNewTimeEdit();
    connect(beginn, SIGNAL(timeChanged(QTime)), this, SLOT(complexWidgetInTableChanged()));
    ui->tablePersonen->setCellWidget(0, 2, beginn);

    QTimeEdit *ende = fahrtag->generateNewTimeEdit();
    connect(ende, SIGNAL(timeChanged(QTime)), this, SLOT(complexWidgetInTableChanged()));
    ui->tablePersonen->setCellWidget(0, 3, ende);

    ui->tablePersonen->setItem(0, 4, new QTableWidgetItem(""));

    ui->buttonRemove->setEnabled(true);
    nehme = nehmeOld;

    widgetInTableToTableWidget.insert(box, item);
    widgetInTableToTableWidget.insert(beginn, item);
    widgetInTableToTableWidget.insert(ende, item);
}

void FahrtagWindow::on_buttonRemove_clicked()
{
    // Prüfen, ob die Zeile wirklich gelöscht werden darf, ansonsten einfach nicht löschen, bzw HInweis in entsprechender Liste löschen
    int i = ui->tablePersonen->currentRow();
    if (i < 0) return;
    QString n = "";
    if (ui->tablePersonen->item(i, 0) != nullptr) {
        if (ui->tablePersonen->item(i, 0)->flags() == Qt::NoItemFlags) {
            QMessageBox::information(this, "Nicht löschbar", "Diese Person müssen Sie in der ensprechenden Liste löschen!");
            return;
        }
        n = ui->tablePersonen->item(i, 0)->text();
    }
    if (fahrtag->removePerson(n)) {
        namen.remove(n);
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

void FahrtagWindow::on_actionResPdf_triggered()
{
    QPrinter *p = Export::getPrinterPDF(this, windowTitle()+"-Reservierungen.pdf");
    Export::printReservierung(fahrtag, p);
}

void FahrtagWindow::on_actionResPrint_triggered()
{
    QPrinter *p = Export::getPrinterPaper(this);
    Export::printReservierung(fahrtag, nullptr, p);
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
    QList<QString> z = r->getZuege();
    QList<QString> h = r->getHps();
    if (z.length() >= 2) {
        ui->comboStart1Zug->setCurrentText(z.at(0));
        ui->comboEnde1Zug->setCurrentText(z.at(1));
        ui->comboStart1Hp->setCurrentText(h.at(0));
        ui->comboEnde1Hp->setCurrentText(h.at(1));
        if (z.length() >= 4) {
            ui->comboStart2Zug->setCurrentText(z.at(2));
            ui->comboEnde2Zug->setCurrentText(z.at(3));
            ui->comboStart2Hp->setCurrentText(h.at(2));
            ui->comboEnde2Hp->setCurrentText(h.at(3));
        } else {
            ui->comboStart2Zug->setCurrentText("-");
            ui->comboEnde2Zug->setCurrentText("-");
            ui->comboStart2Hp->setCurrentText("-");
            ui->comboEnde2Hp->setCurrentText("-");
        }
    } else {
        ui->comboStart1Zug->setCurrentText("-");
        ui->comboEnde1Zug->setCurrentText("-");
        ui->comboStart1Hp->setCurrentText("-");
        ui->comboEnde1Hp->setCurrentText("-");

        ui->comboStart2Zug->setCurrentText("-");
        ui->comboEnde2Zug->setCurrentText("-");
        ui->comboStart2Hp->setCurrentText("-");
        ui->comboEnde2Hp->setCurrentText("-");
    }
    ui->lineSitze->setText(Fahrtag::getStringFromPlaetze(r->getSitzplatz()));
    ui->lineSitze->setStyleSheet("background-color: #b9ceac");
    ui->checkFahrrad->setChecked(r->getFahrrad());
    ui->plainSonstiges->clear();
    ui->plainSonstiges->setPlainText(r->getSonstiges());

    setEnabledFieldsForReservierung(true);
    nehmeRes = true;
}

void FahrtagWindow::saveResFahrt()
{
    QList<QString> z = QList<QString>();
    QList<QString> h = QList<QString>();

    QString zg1 = ui->comboStart1Zug->currentText();
    QString hp1 = ui->comboStart1Hp->currentText();
    QString zg2 = ui->comboEnde1Zug->currentText();
    QString hp2 = ui->comboEnde1Hp->currentText();
    QString zg3 = ui->comboStart2Zug->currentText();
    QString hp3 = ui->comboStart2Hp->currentText();
    QString zg4 = ui->comboEnde2Zug->currentText();
    QString hp4 = ui->comboEnde2Hp->currentText();

    bool i1 = zg1 != "-" || hp1 != "-";
    bool i2 = zg2 != "-" || hp2 != "-";
    bool i3 = zg3 != "-" || hp3 != "-";
    bool i4 = zg4 != "-" || hp4 != "-";
    if (i1 && !i2 && !i3) {
        // append 1 and 4
        z.append(zg1);
        h.append(hp1);
        z.append(zg4);
        h.append(hp4);
    } else if ((i2 && i3) || (i1 && i3) || (i2 && i4)) {
        // append 1, 2, 3, 4
        z.append(zg1);
        h.append(hp1);
        z.append(zg2);
        h.append(hp2);
        z.append(zg3);
        h.append(hp3);
        z.append(zg4);
        h.append(hp4);
    } else if (i1 || i2) {
        // append 1 and 2
        z.append(zg1);
        h.append(hp1);
        z.append(zg2);
        h.append(hp2);
    } else if (i3 || i4) {
        // append 3, 4
        z.append(zg3);
        h.append(hp3);
        z.append(zg4);
        h.append(hp4);
    }
    aktuelleRes->setZuege(z);
    aktuelleRes->setHps(h);
    fahrtag->emitter();
}

void FahrtagWindow::setEnabledFieldsForReservierung(bool enabled)
{
    ui->lineName->setEnabled(enabled);
    ui->lineMail->setEnabled(enabled);
    ui->lineTelefon->setEnabled(enabled);

    ui->spinAnzahl->setEnabled(enabled);
    ui->comboKlasse->setEnabled(enabled);
    ui->lineSitze->setEnabled(enabled);
    ui->checkFahrrad->setEnabled(enabled);

    ui->comboStart1Zug->setEnabled(enabled);
    ui->comboStart1Hp->setEnabled(enabled);
    ui->comboEnde1Zug->setEnabled(enabled);
    ui->comboEnde1Hp->setEnabled(enabled);
    ui->comboStart2Zug->setEnabled(enabled);
    ui->comboStart2Hp->setEnabled(enabled);
    ui->comboEnde2Zug->setEnabled(enabled);
    ui->comboEnde2Hp->setEnabled(enabled);

    ui->plainSonstiges->setEnabled(enabled);
}

void FahrtagWindow::on_buttonAdd_clicked()
{
    Reservierung *r = fahrtag->createReservierung();
    QListWidgetItem *i = new QListWidgetItem(r->getName());
    ui->listRes->insertItem(0, i);
    resToItem.insert(r, i);
    itemToRes.insert(i, r);
    ui->buttonDelete->setEnabled(true);
    loadReservierung(r);
    fahrtag->emitter();
}

void FahrtagWindow::on_buttonDelete_clicked()
{
    // Nachfrage ob wirklcih löschen
    if (QMessageBox::question(this, "Wirklich löschen?", "Möchten Sie die ausgwählte Reservierung unwiderruflich löschen?") == QMessageBox::Yes) {
        QListWidgetItem *i = ui->listRes->takeItem(ui->listRes->currentRow());
        Reservierung *r = itemToRes.value(i);
        fahrtag->removeReservierung(r);
        itemToRes.remove(i);
        resToItem.remove(r);
        if (ui->listRes->count() > 0) {
            loadReservierung(itemToRes.value(ui->listRes->item(0)));
        } else {
            nehmeRes = false;
            ui->buttonDelete->setEnabled(false);
            setEnabledFieldsForReservierung(false);
        }
        update();
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_listRes_itemDoubleClicked(QListWidgetItem *item)
{
    loadReservierung(itemToRes.value(item));
}

void FahrtagWindow::on_buttonShow_clicked()
{
    QMessageBox::information(this, "Ohne Funktion", QString::number(fahrtag->getAnzahl()));
}

void FahrtagWindow::on_buttonVerteile_clicked()
{
    // Informationen, wie lange es gedauert hat
    QTime start = QTime::currentTime();
    QList<Mistake> ok = fahrtag->verteileSitzplaetze();
    QTime ende = QTime::currentTime();
    if (nehme) {
        for(int i = 0; i < ok.length(); ++i) {
            switch (ok.at(i)) {
            case OK:
                if (ok.count(Mistake::OK) != ok.length())
                    QMessageBox::information(this, tr("Fertig"), tr("Die Reservierungen in der ") + (i==0 ? "2. und 3" : QString::number(i)) + tr(". Klasse wurden erfolgreich verteilt."));
                break;
            case KapazitaetUeberlauf:
                QMessageBox::information(this, tr("Fehler"), tr("Die Reservierungen in der ") + (i==0 ? "2. und 3" : QString::number(i)) + tr(". Klasse konnten nicht verteilt werden, da nicht genügend Sitzplätze zur Verfügung stehen!"));
                break;
            default:
                QMessageBox::information(this, tr("Fehler"), tr("Die Reservierungen in der ") + (i==0 ? "2. und 3" : QString::number(i)) + tr(". Klasse konnten nicht verteilt werden!\nBitte prüfen Sie die Eingaben auf mögliche Fehler!"));
                break;
            }
        }
        if (ok.count(Mistake::OK) == ok.length()) {
            QMessageBox::information(this, tr("Fertig"), tr("Die Reservierungen in allen Klassen wurden erfolgreich verteilt."));
        }
        if (CoreApplication::isDeveloperVersion()) {
            QMessageBox::information(this, tr("Fertig"), "mSek: "+QString::number(start.msecsTo(ende)));
        }
        fahrtag->emitter();
        update();
    }
}

void FahrtagWindow::on_lineName_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setName(arg1);
        resToItem.value(aktuelleRes)->setText(aktuelleRes->getName());
        fahrtag->emitter();
        ui->listRes->sortItems();
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
        update();
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

void FahrtagWindow::on_comboStart1Zug_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboStart1Hp_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboEnde1Zug_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboEnde1Hp_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboStart2Zug_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboStart2Hp_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboEnde2Zug_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_comboEnde2Hp_currentIndexChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_lineSitze_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        QMap<int, QList<int>> plaetze = ManagerReservierungen::getPlaetzeFromString(arg1);
        bool ok = fahrtag->checkPlaetze(plaetze, aktuelleRes);
        aktuelleRes->setSitzplatz(plaetze);
        update();
        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        if (ok) {
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
        } else {
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
        }
        ui->lineSitze->repaint();
        fahrtag->emitter();
    }
}

void FahrtagWindow::on_lineSitze_returnPressed()
{
    // prüfe, ob die sitzplätze valide sidn und speichere sie
    if (nehmeRes) {
        QMap<int, QList<int>> plaetze = ManagerReservierungen::getPlaetzeFromString(ui->lineSitze->text());
        bool ok = fahrtag->checkPlaetze(plaetze, aktuelleRes);
        aktuelleRes->setSitzplatz(plaetze);
        update();
        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        if (ok) {
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
        } else {
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
            QMessageBox::information(this, "Sitzplätze fehlerhaft", "Die eingegebenen Sitzplätze sind möglicherweise belegt! Bitte überprüfen Sie ihre Eingabe.");
        }
        ui->lineSitze->repaint();
        fahrtag->emitter();
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
    loadReservierung(itemToRes.value(item));
}

void FahrtagWindow::update()
{
    int belegtErste  = fahrtag->getBelegtErste();
    int belegtZweite = fahrtag->getBelegtZweite();
    int belegtDritte = fahrtag->getBelegtDritte();
    int belegtGesamt = fahrtag->getBelegtGesamt();

    int kapErste  = fahrtag->getCapacityErste();
    int kapZweite = fahrtag->getCapacityZweite();
    int kapDritte = fahrtag->getCapacityDritte();
    int kapGesamt = fahrtag->getCapacityGesamt();

    QString darstellung = "%1/%2 (%3 %)";
    QString erste  = darstellung.arg(belegtErste ).arg(kapErste ).arg(belegtErste *100.0/(kapErste ), 0, 'g', 3);
    QString zweite = darstellung.arg(belegtZweite).arg(kapZweite).arg(belegtZweite*100.0/(kapZweite), 0, 'g', 3);
    QString dritte = darstellung.arg(belegtDritte).arg(kapDritte).arg(belegtDritte*100.0/(kapDritte), 0, 'g', 3);
    QString gesamt = darstellung.arg(belegtGesamt).arg(kapGesamt).arg(belegtGesamt*100.0/(kapGesamt), 0, 'g', 3);
    if (kapErste  == 0) erste  = "-";
    if (kapZweite == 0) zweite = "-";
    if (kapDritte == 0) dritte = "-";
    ui->labelBelegtErste ->setText(erste );
    ui->labelBelegtZweite->setText(zweite);
    ui->labelBelegtDritte->setText(dritte);
    ui->labelBelegtGesamt->setText(gesamt);

    ui->labelBelegtErste->repaint();
    ui->labelBelegtZweite->repaint();
    ui->labelBelegtDritte->repaint();
    ui->labelBelegtGesamt->repaint();
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

void FahrtagWindow::complexWidgetInTableChanged()
{
    if (nehme) {
        QWidget *obj = qobject_cast<QWidget*>(sender());
        if (! widgetInTableToTableWidget.contains(obj)) return;
        QTableWidgetItem *tableItem = widgetInTableToTableWidget.value(obj);
        on_tablePersonen_cellChanged(ui->tablePersonen->row(tableItem), 1);
    }
}
