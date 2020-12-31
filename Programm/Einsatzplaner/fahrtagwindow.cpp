#include "fahrtagwindow.h"
#include "person.h"
#include "ui_fahrtagwindow.h"
#include "export.h"
#include "coreapplication.h"
#include "guihelper.h"

#include <QMessageBox>
#include <QTableWidgetItem>

FahrtagWindow::FahrtagWindow(QWidget *parent, Fahrtag *f) : QMainWindow(parent), ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
    ui->buttonGroupTf->setId(ui->radioButtonTf0, 0);
    ui->buttonGroupTf->setId(ui->radioButtonTf1, 1);
    ui->buttonGroupTf->setId(ui->radioButtonTf2, 2);

    connect(ui->comboStart1Zug, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboStart1Hp, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboEnde1Zug, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboEnde1Hp, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboStart2Zug, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboStart2Hp, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboEnde2Zug, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->comboEnde2Hp, SIGNAL(currentIndexChanged(int)), this, SLOT(handlerFahrtChanged()));
    connect(ui->buttonAddPerson, SIGNAL(clicked()), this, SLOT(fuegeZeileInTabelleEin()));

    fahrtag = f;

    updateWindowTitle();

    nehmeRes = false;
    nehme = false;

    // Allgemeine Daten von AActivity
    ui->dateDate->setDate(fahrtag->getDatum());
    ui->textAnlass->setPlainText(fahrtag->getAnlass().replace("<br/>","\n"));
    ui->comboTimeEndeH->setCurrentText(fahrtag->getZeitEnde().toString("HH"));
    ui->comboTimeEndeM->setCurrentText(fahrtag->getZeitEnde().toString("mm"));
    ui->checkZeiten->setChecked(fahrtag->getZeitenUnbekannt());
    on_checkZeiten_clicked(fahrtag->getZeitenUnbekannt());
    ui->checkBoxBenoetigt->setChecked(fahrtag->getPersonalBenoetigt());
    ui->textBemerkungen->setPlainText(fahrtag->getBemerkungen().replace("<br/>","\n"));

    // Daten von Fahrtag
    ui->comboArt->setCurrentIndex(fahrtag->getArt());
    on_comboArt_currentIndexChanged(fahrtag->getArt());
    ui->comboTimeTfH->setCurrentText(fahrtag->getZeitTf().toString("HH"));
    ui->comboTimeTfM->setCurrentText(fahrtag->getZeitTf().toString("mm"));
    ui->comboTimeZH->setCurrentText(fahrtag->getZeitAnfang().toString("HH"));
    ui->comboTimeZM->setCurrentText(fahrtag->getZeitAnfang().toString("mm"));
    ui->checkWichtig->setChecked(fahrtag->getWichtig());

    ui->buttonGroupTf->button(fahrtag->getBenoetigeTf())->click();
    ui->checkZf->setChecked(fahrtag->getBenoetigeZf());
    ui->checkZub->setChecked(fahrtag->getBenoetigeZub());
    ui->checkService->setChecked(fahrtag->getBenoetigeService());

    // Daten von Manager_Reservierungen
    itemToRes = QMap<QListWidgetItem*, Reservierung*>();
    ui->comboWagenreihung->setCurrentText(fahrtag->getWagenreihung());
    for (Reservierung *r: fahrtag->getReservierungen()) {
        QListWidgetItem *item = new QListWidgetItem(r->getName());
        ui->listRes->insertItem(0, item);
        itemToRes.insert(item, r);
        ui->buttonDeleteReservierung->setEnabled(true);
    }
    ui->listRes->sortItems();

    ui->checkBoxAll->setChecked(fahrtag->getCheckAll());

    /* Personal einfuegen */
    QMap<AActivity::Einsatz, Infos> tf;
    QMap<AActivity::Einsatz, Infos> zf;
    QMap<AActivity::Einsatz, Infos> zub;
    QMap<AActivity::Einsatz, Infos> service;
    QMap<AActivity::Einsatz, Infos> sonstige;

    listeZuTabelle = QMap<QListWidgetItem*, QTableWidgetItem*>();
    tabelleZuEinsatz = QMap<QTableWidgetItem*, AActivity::Einsatz>();

    for(AActivity::Einsatz e: fahrtag->getPersonen().keys()) {
        Infos info = fahrtag->getPersonen().value(e);

        // Fügt die Personene in die einzelnen Listen ein, sodass es direkt geändert werden kann
        QListWidgetItem *item = new QListWidgetItem((e.person)->getName() + (info.bemerkung == "" ? "" : "; "+info.bemerkung));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        bool block = true;
        switch (e.cat) {
        case Category::Tf:
        case Category::Tb:
            tf.insert(e, info);
            ui->listTf->addItem(item);
            ui->buttonTfDelete->setEnabled(true);
            break;
        case Category::Zf:
            zf.insert(e, info);
            ui->listZf->addItem(item);
            ui->buttonZfDelete->setEnabled(true);
            break;
        case Category::Zub:
        case Category::Begleiter:
            zub.insert(e, info);
            ui->listZub->addItem(item);
            ui->buttonZubDelete->setEnabled(true);
            break;
        case Category::Service:
            service.insert(e, info);
            ui->listService->addItem(item);
            ui->buttonServiceDelete->setEnabled(true);
            break;
        default:
            sonstige.insert(e, info);
            ui->buttonRemovePerson->setEnabled(true);
            block = false;
            break;
        }
        int row = fuegeZeileInTabelleEin((e.person)->getName(), (info.kategorie == Category::Begleiter ? Zub : info.kategorie), info.bemerkung, block, info.beginn, info.ende);

        listeZuTabelle.insert(item, ui->tablePersonen->item(row,0));
        tabelleZuEinsatz.insert(ui->tablePersonen->item(row,0), e);
    }
    nehme = true;

    updateAuswertungReservierungen();
}

FahrtagWindow::~FahrtagWindow()
{
    delete ui;
}

void FahrtagWindow::on_dateDate_dateChanged(const QDate &date)
{
    if (nehme)
        fahrtag->setDatum(date);
    updateWindowTitle();
}

void FahrtagWindow::on_comboArt_currentIndexChanged(int index)
{
    Art art = static_cast<Art>(index);
    if (nehme) {
        fahrtag->setArt(art);
    }
    ui->tabReservierungen->setEnabled(art != Schnupperkurs);
    ui->checkZf->setEnabled(art != Schnupperkurs);
    ui->checkZub->setEnabled(art != Schnupperkurs);
    ui->checkService->setEnabled(art != Schnupperkurs);

    ui->checkBoxAll->setEnabled(Nikolauszug == art);
    ui->buttonVerteile->setEnabled(Nikolauszug == art);

    updateWindowTitle();
}

void FahrtagWindow::on_textAnlass_textChanged()
{
    if (nehme)
        fahrtag->setAnlass(ui->textAnlass->toPlainText().replace("\n","<br/>"));
}

void FahrtagWindow::on_checkWichtig_clicked(bool checked)
{
    if (nehme)
        fahrtag->setWichtig(checked);
}

void FahrtagWindow::on_comboWagenreihung_currentTextChanged(const QString &arg1)
{
    if (nehme) {
        QString alt = fahrtag->getWagenreihung();
        if (fahrtag->setWagenreihung(arg1)) {
            updateAuswertungReservierungen();
        } else {
            QMessageBox::information(this, tr("Fehler"), tr("Die Wagenreihung konnte nicht geändert werden, da es Reservierungen in den wegfallenden Waggons gibt!"));
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
        fahrtag->setBemerkungen(ui->textBemerkungen->toPlainText().replace("\n","<br/>"));
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

void FahrtagWindow::on_checkZeiten_clicked(bool checked)
{
    if (nehme)
        fahrtag->setZeitenUnbekannt(checked);
    ui->comboTimeTfH->setEnabled(!checked);
    ui->comboTimeTfM->setEnabled(!checked);
    ui->comboTimeZH->setEnabled(!checked);
    ui->comboTimeZM->setEnabled(!checked);
    ui->comboTimeEndeH->setEnabled(!checked);
    ui->comboTimeEndeM->setEnabled(!checked);
}

void FahrtagWindow::addItemTolist(QListWidget *l, QPushButton *b)
{
    if (nehme) {
        nehme = false;
        QListWidgetItem *item = new QListWidgetItem(tr("(Name eingeben)"));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        l->addItem(item);
        b->setEnabled(true);
        nehme = true;
    }
}
void FahrtagWindow::deleteItemFromList(QListWidget *l, QPushButton *b)
{
    QListWidgetItem *item = l->currentItem();
    if (item == nullptr) return;
    if (listeZuTabelle.contains(item)) {
        if (tabelleZuEinsatz.contains(listeZuTabelle.value(item))) {
            AActivity::Einsatz e = tabelleZuEinsatz.value(listeZuTabelle.value(item));
            fahrtag->removePerson(e.person, e.cat);
        }
        ui->tablePersonen->removeRow(ui->tablePersonen->row(listeZuTabelle.value(item)));
        tabelleZuEinsatz.remove(listeZuTabelle.value(item));
        listeZuTabelle.remove(item);
    }
    l->removeItemWidget(item);
    delete item;
    b->setEnabled(l->count()>0);
}

void FahrtagWindow::itemInListChanged(QListWidgetItem *item , Category kat)
{
    /* item: Das Item das Verändert wurde */
    /* kat: Die Kategorie, auf die gebucht wird */
    QString name;
    QString bemerkung;
    {
        QStringList liste = item->text().split(QRegExp("\\s*;\\s*"));
        name = liste.at(0);
        liste.removeFirst();
        bemerkung = liste.join("; ");
    }

    if (kat == Tf && bemerkung.contains(getLocalizedStringFromCategory(Tb), Qt::CaseInsensitive))
        kat = Tb;
    if (kat == Zub && !AActivity::hasQualification(fahrtag->getPersonal()->getPerson(name), kat, bemerkung))
        kat = Begleiter;

    if (listeZuTabelle.contains(item)) {
        // Person wurde bereits eingetragen, entweder intern oder extern
        AActivity::Einsatz e = tabelleZuEinsatz.value(listeZuTabelle.value(item));
        if (name == (e.person)->getName()) {
            // Bemerkung wurde lediglich verändert
            fahrtag->updatePersonBemerkung(e.person, e.cat, bemerkung);
            ui->tablePersonen->item(ui->tablePersonen->row(listeZuTabelle.value(item)), 4)->setText(bemerkung);
            return;
        }
        // Name wurde veraendert, somit ist die Person zu entfernen und neu einzufuegen
        fahrtag->removePerson(e.person, e.cat);

        ui->tablePersonen->removeRow(ui->tablePersonen->row(listeZuTabelle.value(item)));
        tabelleZuEinsatz.remove(listeZuTabelle.value(item));
        listeZuTabelle.remove(item);
    }

    QTime beginn = QTime(0,0);
    QTime ende = QTime(0,0);
    int row;
    switch (fahrtag->addPerson(name, bemerkung, beginn, ende, kat)) {
    case Mistake::PassivOk:
        QMessageBox::information(this, tr("Information"), tr("Die Person wird als passives Mitglied geführt. Sie wurde aber dennoch eingetragen!"));
        [[fallthrough]];
    case Mistake::ExternOk:
    case Mistake::OK:
        // Zeile für die Person in die Tabelle einfügen
        row = fuegeZeileInTabelleEin(name, kat, bemerkung, true);

        listeZuTabelle.insert(item, ui->tablePersonen->item(row, 0));
        tabelleZuEinsatz.insert(ui->tablePersonen->item(row,0), AActivity::Einsatz{fahrtag->getPerson(name), kat});
        break;

    case Mistake::FalscheQualifikation:
        QMessageBox::warning(this, tr("Fehlende Qualifikation"), tr("Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da eine betriebliche Ausbildung und gültige Tauglichkeitsuntersuchung benötigt wird."));
        break;
    case Mistake::PersonNichtGefunden:
        QMessageBox::information(this, tr("Person nicht gefunden"), tr("Die eingegebene Person konnte nicht gefunden werden!"));
        break;
    default:
        QMessageBox::information(this, tr("Fehler"), tr("Beim Hinzufügen der Person zum Fahrtag ist ein Fehler aufgetreten! Bitte überprüfen Sie ihre Eingaben!"));
        break;
    }
}

void FahrtagWindow::plausibilityCheck()
{
    int z1 = getZugVonIndex(ui->comboStart1Zug->currentIndex());
    int z2 = getZugVonIndex(ui->comboEnde1Zug->currentIndex());
    int z3 = getZugVonIndex(ui->comboStart2Zug->currentIndex());
    int z4 = getZugVonIndex(ui->comboEnde2Zug->currentIndex());

    int h1 = getHpVonIndex(ui->comboStart1Hp->currentIndex());
    int h2 = getHpVonIndex(ui->comboEnde1Hp->currentIndex());
    int h3 = getHpVonIndex(ui->comboStart2Hp->currentIndex());
    int h4 = getHpVonIndex(ui->comboEnde2Hp->currentIndex());

    if (! fahrtag->checkPlausibilitaet({z1, z2, z3, z4}, {h1, h2, h3, h4})) {
        QMessageBox::warning(this, tr("Plausibilitätsprüfung"), tr("Bitte überprüfen Sie ihre Eingaben bezüglich der Fahrstrecken, da das System eine mögliche Unstimmigkeit festgestllt hat. Ihre Daten werden dennoch gespeichert!"));
    }
}

void FahrtagWindow::on_listTf_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        itemInListChanged(item, Category::Tf);
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
void FahrtagWindow::on_buttonGroupTf_buttonClicked(int button)
{
    if (nehme)
        fahrtag->setBenoetigeTf(button);
}

void FahrtagWindow::on_listZf_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        itemInListChanged(item, Category::Zf);
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
    if (nehme)
        fahrtag->setBenoetigeZf(checked);
}

void FahrtagWindow::on_listZub_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        itemInListChanged(item, Category::Zub);
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
    if (nehme)
        fahrtag->setBenoetigeZub(checked);
}

void FahrtagWindow::on_listService_itemChanged(QListWidgetItem *item)
{
    if (nehme) {
        nehme = false;
        itemInListChanged(item, Category::Service);
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
    if (nehme)
        fahrtag->setBenoetigeService(checked);
}

void FahrtagWindow::on_tablePersonen_cellChanged(int row, int column)
{
    ui->tablePersonen->resizeColumnsToContents();
    if (nehme) {
        nehme = false;

        QTableWidgetItem *item = ui->tablePersonen->item(row, 0);
        QString name = item->text();
        Category kat = getCategoryFromLocalizedString(static_cast<QComboBox*>(ui->tablePersonen->cellWidget(row, 1))->currentText());
        QTime beginn = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 2))->time();
        QTime ende = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 3))->time();
        QString bemerkung = (ui->tablePersonen->item(row, 4) == nullptr) ? "" :  ui->tablePersonen->item(row,4)->text();

        AActivity::Einsatz e = tabelleZuEinsatz.value(item);
        Infos neu = Infos{beginn, ende, kat, bemerkung};
        bool neuHinzufuegen = false;

        if (column == 0 || column == 1) {
            neuHinzufuegen = true;
        } else if (column == 2 || column == 3) {
            fahrtag->updatePersonInfos(e.person, e.cat, neu);
        } else if (column == 4) {
            fahrtag->updatePersonBemerkung(e.person, e.cat, bemerkung);
        }
        if (neuHinzufuegen) {
            if (e.person != nullptr)
                fahrtag->removePerson(e.person, e.cat);

            if (kat == Zub && !AActivity::hasQualification(fahrtag->getPersonal()->getPerson(name), kat, bemerkung))
                kat = Begleiter;

            switch (fahrtag->addPerson(name, bemerkung, beginn, ende, kat)) {
            case Mistake::PassivOk:
                QMessageBox::information(this, tr("Information"), tr("Die Person wird als passives Mitglied geführt. Sie wurde aber dennoch eingetragen!"));
                [[fallthrough]];
            case Mistake::OK:
            case Mistake::ExternOk:
                break;
            case Mistake::PersonNichtGefunden:
                QMessageBox::warning(this, tr("Person nicht gefunden"), tr("Die eingegebene Person konnte nicht gefunden werden!"));
                break;
            case Mistake::FalscheQualifikation:
                QMessageBox::warning(this, tr("Fehlene Qualifikation"), tr("Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da eine betriebliche Ausbildung und gültige Tauglichkeitsuntersuchung benötigt wird."));
                break;
            default:
                QMessageBox::warning(this, tr("Sonstiger Fehler"), tr("Während der Verarbeitung der Eingabe ist ein Fehler unterlaufen.\nPrüfen Sie Ihre Eingaben und versuchen es erneut!"));
                break;
            }
            tabelleZuEinsatz.insert(item, AActivity::Einsatz{fahrtag->getPerson(name), kat});
        }
        nehme = true;
    }
}


void FahrtagWindow::on_buttonRemovePerson_clicked()
{
    // Prüfen, ob die Zeile wirklich gelöscht werden darf, ansonsten einfach nicht löschen, bzw HInweis in entsprechender Liste löschen
    int i = ui->tablePersonen->currentRow();
    if (i < 0) return;
    if (ui->tablePersonen->item(i, 0)->flags() == Qt::NoItemFlags) {
        QMessageBox::information(this, tr("Nicht löschbar"), tr("Diese Person müssen Sie in der ensprechenden Liste löschen!"));
        return;
    }
    QTableWidgetItem *item = ui->tablePersonen->item(i, 0);
    if (tabelleZuEinsatz.contains(item)) {
        AActivity::Einsatz e = tabelleZuEinsatz.value(item);
        fahrtag->removePerson(e.person, e.cat);
    }

    tabelleZuEinsatz.remove(item);
    ui->tablePersonen->removeRow(i);
    ui->buttonRemovePerson->setEnabled(ui->tablePersonen->rowCount() > 0);
}

void FahrtagWindow::on_actionDelete_triggered()
{
    if (QMessageBox::question(this, tr("Wirklich löschen?"), tr("Möchten Sie diesen Fahrtag wirklich unwiderruflich löschen?")) == QMessageBox::Yes) {
        emit fahrtag->del(fahrtag);
        this->close();
        deleteLater();
    }
}

void FahrtagWindow::on_actionPrint_triggered()
{
    Export::printAktivitaetenEinzel({fahrtag},
                                    Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}
void FahrtagWindow::on_actionPdf_triggered()
{
    Export::printAktivitaetenEinzel({fahrtag},
                                    Export::getPrinterPDF(this, windowTitle()+".pdf", QPrinter::Orientation::Portrait));
}

void FahrtagWindow::on_actionResPrint_triggered()
{
    Export::printReservierung(fahrtag,
                              Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}
void FahrtagWindow::on_actionResPdf_triggered()
{
    Export::printReservierung(fahrtag,
                              Export::getPrinterPDF(this, windowTitle()+"-Reservierungen.pdf", QPrinter::Orientation::Portrait));
}

void FahrtagWindow::showReservierung(Reservierung *r)
{
    nehmeRes = false;
    aktuelleRes = r;
    ui->lineName->setText(r->getName());
    ui->lineMail->setText(r->getMail());
    ui->lineTelefon->setText(r->getTelefon());
    ui->spinAnzahl->setValue(r->getAnzahl());
    ui->comboKlasse->setCurrentIndex(r->getKlasse());
    QList<int> z = r->getZuege();
    QList<QString> h = r->getHps();


    ui->comboStart1Zug->setCurrentText("-");
    ui->comboStart1Hp->setCurrentText("-");
    ui->comboEnde1Zug->setCurrentText("-");
    ui->comboEnde1Hp->setCurrentText("-");

    ui->comboStart2Zug->setCurrentText("-");
    ui->comboStart2Hp->setCurrentText("-");
    ui->comboEnde2Zug->setCurrentText("-");
    ui->comboEnde2Hp->setCurrentText("-");
    if (z.length() >= 2) {
        ui->comboStart1Zug->setCurrentIndex(getIndexVonZug(z.at(0)));
        ui->comboStart1Hp->setCurrentText(h.at(0));
        ui->comboEnde1Zug->setCurrentIndex(getIndexVonZug(z.at(1)));
        ui->comboEnde1Hp->setCurrentText(h.at(1));
    }
    if (z.length() >= 4) {
        ui->comboStart2Zug->setCurrentIndex(getIndexVonZug(z.at(2)));
        ui->comboStart2Hp->setCurrentText(h.at(2));
        ui->comboEnde2Zug->setCurrentIndex(getIndexVonZug(z.at(3)));
        ui->comboEnde2Hp->setCurrentText(h.at(3));
    }
    ui->lineSitze->setText(Reservierung::getStringFromPlaetze(r->getSitzplatz()));
    ui->checkFahrrad->setChecked(r->getFahrrad());
    ui->plainSonstiges->setPlainText(r->getSonstiges());

    toggleFelderReservierung(true);
    nehmeRes = true;
}

void FahrtagWindow::saveResFahrt()
{
    QList<int> z = QList<int>();
    QList<QString> h = QList<QString>();

    z.append(getZugVonIndex(ui->comboStart1Zug->currentIndex()));
    h.append(ui->comboStart1Hp->currentText());
    z.append(getZugVonIndex(ui->comboEnde1Zug->currentIndex()));
    h.append(ui->comboEnde1Hp->currentText());
    z.append(getZugVonIndex(ui->comboStart2Zug->currentIndex()));
    h.append(ui->comboStart2Hp->currentText());
    z.append(getZugVonIndex(ui->comboEnde2Zug->currentIndex()));
    h.append(ui->comboEnde2Hp->currentText());

    aktuelleRes->setZuege(z);
    aktuelleRes->setHps(h);

    updateAuswertungReservierungen();
}

void FahrtagWindow::toggleFelderReservierung(bool enabled)
{
    ui->lineName->setEnabled(enabled);
    ui->lineMail->setEnabled(enabled);
    ui->lineTelefon->setEnabled(enabled);

    ui->spinAnzahl->setEnabled(enabled);
    ui->comboKlasse->setEnabled(enabled);
    ui->checkFahrrad->setEnabled(enabled);
    ui->lineSitze->setEnabled(enabled);

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

void FahrtagWindow::updateWindowTitle()
{
    setWindowTitle(getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum().toString("dddd, dd.MM.yyyy"));
}

int FahrtagWindow::getIndexVonZug(int zug)
{
    if (zug == 0)
        return 0;
    return zug - 2200;
}
int FahrtagWindow::getZugVonIndex(int index)
{
    if (index == 0)
        return 0;
    return index + 2200;
}

int FahrtagWindow::getHpVonIndex(int index)
{
    if (index == 11) {
        return -1;
    } else {
        return index;
    }
}

QString FahrtagWindow::getBelegungVonKlasseUndZug(Fahrtag *f, int zug, int klasse)
{
    QString d = "%1 (%2%)";
    d = "%1";
    d = d.arg(f->getBelegung(klasse, zug));
//    d = d.arg(100./f->getKapazitaet(klasse)*f->getBelegung(klasse, zug),0,'g',3);
    return d;
}

int FahrtagWindow::fuegeZeileInTabelleEin(QString name, Category kat, QString bemerkung, bool block, QTime beginn, QTime ende)
{
    bool nehmeOld = nehme;
    nehme = false;
    ui->tablePersonen->insertRow(0);

    QTableWidgetItem *item = new QTableWidgetItem(name);
    ui->tablePersonen->setItem(0, 0, item);

    QComboBox *box = generateNewCategoryComboBox();
    box->setCurrentText(getLocalizedStringFromCategory(kat));
    ui->tablePersonen->setCellWidget(0, 1, box);

    QTimeEdit *beginnEdit = generateNewTimeEdit();
    beginnEdit->setTime(beginn);
    ui->tablePersonen->setCellWidget(0, 2, beginnEdit);

    QTimeEdit *endeEdit = generateNewTimeEdit();
    endeEdit->setTime(ende);
    ui->tablePersonen->setCellWidget(0, 3, endeEdit);

    ui->tablePersonen->setItem(0, 4, new QTableWidgetItem(bemerkung));

    if (block) {
        ui->tablePersonen->item(0, 0)->setFlags(Qt::NoItemFlags);
        ui->tablePersonen->cellWidget(0, 1)->setDisabled(block);
        ui->tablePersonen->item(0, 4)->setFlags(Qt::NoItemFlags);
    }

    connect(box, &QComboBox::currentTextChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 1); });
    connect(beginnEdit, &QTimeEdit::timeChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 2); });
    connect(endeEdit, &QTimeEdit::timeChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 3); });

    tabelleZuEinsatz.insert(item, AActivity::Einsatz{nullptr, kat});

    ui->buttonRemovePerson->setEnabled(true);
    nehme = nehmeOld;
    return ui->tablePersonen->row(item);
}

void FahrtagWindow::on_buttonAddReservierung_clicked()
{
    Reservierung *r = fahrtag->createReservierung();
    QListWidgetItem *i = new QListWidgetItem(r->getName());
    ui->listRes->insertItem(0, i);
    itemToRes.insert(i, r);
    ui->buttonDeleteReservierung->setEnabled(true);
    showReservierung(r);
}

void FahrtagWindow::on_buttonDeleteReservierung_clicked()
{
    if (QMessageBox::question(this, tr("Wirklich löschen?"), tr("Möchten Sie die ausgewählte Reservierung unwiderruflich löschen?")) == QMessageBox::Yes) {
        QListWidgetItem *i = ui->listRes->takeItem(ui->listRes->currentRow());
        Reservierung *r = itemToRes.value(i);
        fahrtag->removeReservierung(r);
        itemToRes.remove(i);
        delete i;

        nehmeRes = false;
        ui->buttonDeleteReservierung->setEnabled(ui->listRes->count());
        toggleFelderReservierung(false);

        updateAuswertungReservierungen();
    }
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
        if (CoreApplication::DEVELOPER_MODE) {
            QMessageBox::information(this, tr("Fertig"), "mSek: "+QString::number(start.msecsTo(ende)));
        }
        updateAuswertungReservierungen();
    }
}

void FahrtagWindow::on_lineName_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setName(arg1);
        itemToRes.key(aktuelleRes)->setText(arg1);
        ui->listRes->sortItems();
    }
}

void FahrtagWindow::on_lineMail_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setMail(arg1);
    }
}

void FahrtagWindow::on_lineTelefon_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        aktuelleRes->setTelefon(arg1);
    }
}

void FahrtagWindow::on_spinAnzahl_valueChanged(int arg1)
{
    if (nehmeRes) {
        aktuelleRes->setAnzahl(arg1);
        updateAuswertungReservierungen();
    }
}

void FahrtagWindow::on_comboKlasse_currentIndexChanged(int index)
{
    if (nehmeRes) {
        aktuelleRes->setKlasse(index);
        updateAuswertungReservierungen();
    }
}

void FahrtagWindow::on_lineSitze_textChanged(const QString &arg1)
{
    if (nehmeRes) {
        bool ok = fahrtag->checkPlaetze(arg1, aktuelleRes);
        aktuelleRes->setSitzplatz(arg1);
        updateAuswertungReservierungen();
        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        if (ok) {
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
        } else {
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
        }
        ui->lineSitze->repaint();
    }
}

void FahrtagWindow::on_lineSitze_returnPressed()
{
    // prüfe, ob die sitzplätze valide sidn und speichere sie
    if (nehmeRes) {
        QString arg1 = ui->lineSitze->text();
        bool ok = fahrtag->checkPlaetze(arg1, aktuelleRes);
        aktuelleRes->setSitzplatz(arg1);
        updateAuswertungReservierungen();
        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        if (ok) {
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
        } else {
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
            QMessageBox::information(this, tr("Sitzplätze fehlerhaft"), tr("Die eingegebenen Sitzplätze sind möglicherweise belegt! Bitte überprüfen Sie Ihre Eingabe."));
        }
        ui->lineSitze->repaint();
    }
}

void FahrtagWindow::on_checkFahrrad_clicked(bool checked)
{
    if (nehmeRes) {
        aktuelleRes->setFahrrad(checked);
    }
}

void FahrtagWindow::handlerFahrtChanged()
{
    if (nehmeRes) {
        plausibilityCheck();
        saveResFahrt();
    }
}

void FahrtagWindow::on_plainSonstiges_textChanged()
{
    if (nehmeRes) {
        aktuelleRes->setSonstiges(ui->plainSonstiges->toPlainText());
    }
}

void FahrtagWindow::on_listRes_itemClicked(QListWidgetItem *item)
{
    showReservierung(itemToRes.value(item));
}

void FahrtagWindow::updateAuswertungReservierungen()
{
    ui->labelKapErste->setNum(fahrtag->getKapazitaet(1));
    ui->label2201Erste->setText(getBelegungVonKlasseUndZug(fahrtag, 2201, 1));
    ui->label2202Erste->setText(getBelegungVonKlasseUndZug(fahrtag, 2202, 1));
    ui->label2203Erste->setText(getBelegungVonKlasseUndZug(fahrtag, 2203, 1));
    ui->label2204Erste->setText(getBelegungVonKlasseUndZug(fahrtag, 2204, 1));
    ui->label2205Erste->setText(getBelegungVonKlasseUndZug(fahrtag, 2205, 1));
    ui->label2206Erste->setText(getBelegungVonKlasseUndZug(fahrtag, 2206, 1));
    ui->labelGesamtErste->setNum(fahrtag->getBelegung(1));

    ui->labelKapSonst->setNum(fahrtag->getKapazitaet(0));
    ui->label2201Sonst->setText(getBelegungVonKlasseUndZug(fahrtag, 2201, 0));
    ui->label2202Sonst->setText(getBelegungVonKlasseUndZug(fahrtag, 2202, 0));
    ui->label2203Sonst->setText(getBelegungVonKlasseUndZug(fahrtag, 2203, 0));
    ui->label2204Sonst->setText(getBelegungVonKlasseUndZug(fahrtag, 2204, 0));
    ui->label2205Sonst->setText(getBelegungVonKlasseUndZug(fahrtag, 2205, 0));
    ui->label2206Sonst->setText(getBelegungVonKlasseUndZug(fahrtag, 2206, 0));
    ui->labelGesamtSonst->setNum(fahrtag->getBelegung(0));

    ui->labelKapSum->setNum(fahrtag->getKapazitaet(-1));
    ui->label2201Sum->setText(getBelegungVonKlasseUndZug(fahrtag, 2201, -1));
    ui->label2202Sum->setText(getBelegungVonKlasseUndZug(fahrtag, 2202, -1));
    ui->label2203Sum->setText(getBelegungVonKlasseUndZug(fahrtag, 2203, -1));
    ui->label2204Sum->setText(getBelegungVonKlasseUndZug(fahrtag, 2204, -1));
    ui->label2205Sum->setText(getBelegungVonKlasseUndZug(fahrtag, 2205, -1));
    ui->label2206Sum->setText(getBelegungVonKlasseUndZug(fahrtag, 2206, -1));
    ui->labelGesamtSum->setNum(fahrtag->getBelegung(-1));

    on_comboAuswahlRes_currentIndexChanged(ui->comboAuswahlRes->currentIndex());
}

void FahrtagWindow::on_checkBoxBenoetigt_clicked(bool checked)
{
    if (nehme) {
        fahrtag->setPersonalBenoetigt(checked);
    }
}

void FahrtagWindow::on_checkBoxAll_clicked(bool checked)
{
    if (nehme){
        if (checked) QMessageBox::information(this, tr("Hinweis"), tr("Es kann unter Umständen sehr lange dauern, bis die 'optimale' Verteilung berechnet wird.\nIhr Computer reagiert in dieser Zeit vielleicht nicht!\nDiese Funktion sollten Sie nur für eine kleine Anzahl an Reservierungen benutzen."));
        fahrtag->setCheckAll(checked);
    }
}

void FahrtagWindow::on_comboAuswahlRes_currentIndexChanged(int index)
{
    QListWidgetItem *item;
    for(int i = 0; i < ui->listRes->count(); ++i) {
        item = ui->listRes->item(i);
        if (index == 0) {
            item->setHidden(false);
        } else {
            item->setHidden(! itemToRes.value(item)->inZug(index+2200));
        }
    }
    ui->listRes->repaint();
}
