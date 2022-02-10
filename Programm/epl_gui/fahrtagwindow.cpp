#include "fahrtagwindow.h"
#include "person.h"
#include "ui_fahrtagwindow.h"
#include "export.h"
#include "coreapplication.h"
#include "guihelper.h"
#include "eplexception.h"

#include <QMessageBox>
#include <QTableWidgetItem>

using namespace EplException;

FahrtagWindow::FahrtagWindow(CoreMainWindow *parent, Fahrtag *f) : QMainWindow(parent), ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
    ui->buttonGroupTf->setId(ui->radioButtonTf0, 0);
    ui->buttonGroupTf->setId(ui->radioButtonTf1, 1);
    ui->buttonGroupTf->setId(ui->radioButtonTf2, 2);

    connect(this, &FahrtagWindow::loeschen, parent, &CoreMainWindow::loeschenAktivitaet);

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
    ui->lineAnlass->setText(fahrtag->getAnlass().replace("<br/>","\n"));
    ui->timeEnde->setTime(fahrtag->getZeitEnde());
    ui->checkZeiten->setChecked(fahrtag->getZeitenUnbekannt());
    on_checkZeiten_clicked(fahrtag->getZeitenUnbekannt());
    ui->checkBoxBenoetigt->setChecked(fahrtag->getPersonalBenoetigt());
    ui->textBemerkungen->setPlainText(fahrtag->getBemerkungen().replace("<br/>","\n"));

    // Daten von Fahrtag
    ui->comboArt->setCurrentIndex(fahrtag->getArt());
    on_comboArt_currentIndexChanged(fahrtag->getArt());
    ui->timeBeginnTf->setTime(fahrtag->getZeitAnfang(Tf));
    ui->timeBeginn->setTime(fahrtag->getZeitAnfang());
    ui->checkWichtig->setChecked(fahrtag->getWichtig());
    ui->checkAbgesagt->setChecked(fahrtag->getAbgesagt());

    ui->buttonGroupTf->button(fahrtag->getPersonalBenoetigt(Tf))->click();
    ui->checkZf->setChecked(fahrtag->getPersonalBenoetigt(Zf));
    ui->checkZub->setChecked(fahrtag->getPersonalBenoetigt(Zub));
    ui->checkService->setChecked(fahrtag->getPersonalBenoetigt(Service));

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

    for(Einsatz *e: fahrtag->getPersonen()) {
        // Fügt die Personene in die einzelnen Listen ein, sodass es direkt geändert werden kann
        TableListWidgetItem *item = new TableListWidgetItem((e->getPerson())->getName() + (e->getBemerkung() == "" ? "" : "; "+e->getBemerkung()));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        bool block = true;
        switch (e->getKategorie()) {
        case Category::Tf:
        case Category::Tb:
            ui->listTf->addItem(item);
            ui->buttonTfDelete->setEnabled(true);
            break;
        case Category::Zf:
            ui->listZf->addItem(item);
            ui->buttonZfDelete->setEnabled(true);
            break;
        case Category::Zub:
        case Category::Begleiter:
            ui->listZub->addItem(item);
            ui->buttonZubDelete->setEnabled(true);
            break;
        case Category::Service:
            ui->listService->addItem(item);
            ui->buttonServiceDelete->setEnabled(true);
            break;
        default:
            ui->buttonRemovePerson->setEnabled(true);
            block = false;
            break;
        }
        EinsatzTableWidgetItem *ptwi = fuegeInTabelleEin(e, block);
        ptwi->setEinsatz(e);
        item->setTableItem(ptwi);
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

void FahrtagWindow::on_lineAnlass_textChanged()
{
    if (nehme)
        fahrtag->setAnlass(ui->lineAnlass->text().replace("\n","<br/>"));
}

void FahrtagWindow::on_checkWichtig_clicked(bool checked)
{
    if (nehme)
        fahrtag->setWichtig(checked);
}

void FahrtagWindow::on_checkAbgesagt_clicked(bool checked)
{
    if (nehme)
        fahrtag->setAbgesagt(checked);
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

void FahrtagWindow::on_timeBeginnTf_timeChanged(const QTime &time)
{
    if (nehme)
        fahrtag->setZeitAnfang(time, Tf);
}

void FahrtagWindow::on_timeBeginn_timeChanged(const QTime &time)
{
    if (nehme)
        fahrtag->setZeitAnfang(time);
}

void FahrtagWindow::on_textBemerkungen_textChanged()
{
    if (nehme)
        fahrtag->setBemerkungen(ui->textBemerkungen->toPlainText().replace("\n","<br/>"));
}

void FahrtagWindow::on_timeEnde_timeChanged(const QTime &time)
{
    if (nehme)
        fahrtag->setZeitEnde(time);
}

void FahrtagWindow::on_checkZeiten_clicked(bool checked)
{
    if (nehme)
        fahrtag->setZeitenUnbekannt(checked);
    ui->timeBeginnTf->setEnabled(!checked);
    ui->timeBeginn->setEnabled(!checked);
    ui->timeEnde->setEnabled(!checked);
}

void FahrtagWindow::addItemTolist(QListWidget *l, QPushButton *b)
{
    if (nehme) {
        nehme = false;
        QListWidgetItem *item = new TableListWidgetItem(tr("(Name eingeben)"));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
        l->addItem(item);
        b->setEnabled(true);
        nehme = true;
    }
}
void FahrtagWindow::deleteItemFromList(QListWidget *l, QPushButton *b)
{
    if (l->currentItem() == nullptr) return;
    TableListWidgetItem *item = static_cast<TableListWidgetItem*>(l->currentItem());

    if (item->getTableItem() != nullptr) {

        Einsatz *e = item->getTableItem()->getEinsatz();
        fahrtag->removePerson(e);

        ui->tablePersonen->removeRow(ui->tablePersonen->row(item->getTableItem()));
    }
    l->removeItemWidget(item);
    delete item;
    b->setEnabled(l->count()>0);
}

void FahrtagWindow::itemInListChanged(QListWidgetItem *item , Category kat)
{
    /* item: Das Item das Verändert wurde */
    /* kat: Die Kategorie, auf die gebucht wird */
    QStringList liste = item->text().split(QRegularExpression("\\s*;\\s*"));
    QString name = liste.at(0);
    liste.removeFirst();
    QString bemerkung = liste.join("; ");

    TableListWidgetItem *tlwi = static_cast<TableListWidgetItem*>(item);
    if (tlwi->getTableItem() != nullptr) {
        // Person wurde bereits eingetragen, entweder intern oder extern
        Einsatz *e = tlwi->getTableItem()->getEinsatz();
        if (name == e->getPerson()->getName()) {
            // Bemerkung wurde lediglich verändert
            e->setBemerkung(bemerkung);
            ui->tablePersonen->item(ui->tablePersonen->row(tlwi->getTableItem()), 4)->setText(bemerkung);
            return;
        }
        // Name wurde veraendert, somit ist die Person zu entfernen und neu einzufuegen
        fahrtag->removePerson(e);

        ui->tablePersonen->removeRow(ui->tablePersonen->row(tlwi->getTableItem()));
        tlwi->setTableItem(nullptr);
    }

    try {
        Einsatz *einsatz = fahrtag->addPerson(name, bemerkung, kat);
        if (!einsatz->getPerson()->getAktiv())
            QMessageBox::information(this, tr("Information"), tr("Die Person wird als passives Mitglied geführt. Sie wurde aber dennoch eingetragen!"));

        // Zeile für die Person in die Tabelle einfügen
        EinsatzTableWidgetItem *ptwi = fuegeInTabelleEin(einsatz, true);
        ptwi->setEinsatz(einsatz);
        tlwi->setTableItem(ptwi);
    }  catch (AActivityException &e) {
        QMessageBox::warning(this, tr("Fehler"), e.getError());
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
void FahrtagWindow::on_buttonGroupTf_idClicked(int button)
{
    if (nehme)
        fahrtag->setPersonalBenoetigt(button, Tf);
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
        fahrtag->setPersonalBenoetigt(checked?1:0, Zf);
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
        fahrtag->setPersonalBenoetigt(checked?-1:0, Zub);
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
        fahrtag->setPersonalBenoetigt(checked?-1:0, Service);
}

void FahrtagWindow::on_tablePersonen_cellChanged(int row, [[maybe_unused]] int column)
{
    ui->tablePersonen->resizeColumnsToContents();
    if (nehme) {
        nehme = false;

        EinsatzTableWidgetItem *ptwi = static_cast<EinsatzTableWidgetItem*>(ui->tablePersonen->item(row, 0));
        fahrtag->removePerson(ptwi->getEinsatz());

        QString name = ptwi->text();
        Category kat = getCategoryFromLocalizedString(static_cast<QComboBox*>(ui->tablePersonen->cellWidget(row, 1))->currentText());
        QTime beginn = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 2))->time();
        QTime ende = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 3))->time();
        QString bemerkung = (ui->tablePersonen->item(row, 4) == nullptr) ? "" :  ui->tablePersonen->item(row,4)->text();

        try {
            Einsatz *e = fahrtag->addPerson(name, bemerkung, kat);
            e->setBeginnAbweichend(beginn);
            e->setEndeAbweichend(ende);
            ptwi->setEinsatz(e);
            if (! e->getPerson()->getAktiv())
                QMessageBox::information(this, tr("Information"), tr("Die Person wird als passives Mitglied geführt. Sie wurde aber dennoch eingetragen!"));
        }  catch (AActivityException &e) {
            QMessageBox::warning(this, tr("Fehler"), e.getError());
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
    EinsatzTableWidgetItem *ptwi = static_cast<EinsatzTableWidgetItem*>(ui->tablePersonen->item(i, 0));
    Einsatz *e = ptwi->getEinsatz();
    fahrtag->removePerson(e);

    ui->tablePersonen->removeRow(i);
    ui->buttonRemovePerson->setEnabled(ui->tablePersonen->rowCount() > 0);
}

EinsatzTableWidgetItem *FahrtagWindow::fuegeInTabelleEin(Einsatz *e, bool block)
{
    if (e == nullptr)
        return nullptr;

    bool nehmeOld = nehme;
    nehme = false;
    EinsatzTableWidgetItem *etwi = fuegeZeileInTabelleEin();

    int row = ui->tablePersonen->row(etwi);

    etwi->setText(e->getPerson()->getName());
    static_cast<QComboBox*>(ui->tablePersonen->cellWidget(row, 1))->setCurrentText(
                toString((e->getKategorie() == Begleiter ? Zub : e->getKategorie()))
                );
    static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 2))->setTime(e->getBeginnAbweichend());
    static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 3))->setTime(e->getEndeAbweichend());
    if (ui->tablePersonen->item(row, 4) == nullptr) {
        ui->tablePersonen->setItem(row, 4, new QTableWidgetItem(""));
    }
    ui->tablePersonen->item(row,4)->setText(e->getBemerkung());

    if (block) {
        ui->tablePersonen->item(0, 0)->setFlags(Qt::NoItemFlags);
        ui->tablePersonen->cellWidget(0, 1)->setDisabled(block);
        ui->tablePersonen->item(0, 4)->setFlags(Qt::NoItemFlags);
    }

    ui->buttonRemovePerson->setEnabled(true);
    nehme = nehmeOld;
    return etwi;
}

void FahrtagWindow::on_actionDelete_triggered()
{
    emit loeschen(fahrtag);
}

void FahrtagWindow::on_actionPrint_triggered()
{
    fahrtag->print(Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}
void FahrtagWindow::on_actionPdf_triggered()
{
    fahrtag->print(Export::getPrinterPDF(this, windowTitle(), QPageLayout::Orientation::Portrait));
}

void FahrtagWindow::on_actionResPrint_triggered()
{
    fahrtag->printReservierungsuebersicht(Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}
void FahrtagWindow::on_actionResPdf_triggered()
{
    fahrtag->printReservierungsuebersicht(Export::getPrinterPDF(this, windowTitle()+"-Reservierungen", QPageLayout::Orientation::Portrait));
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
    setWindowTitle(toString(fahrtag->getArt())+" am "+QLocale().toString(fahrtag->getDatum(), "dddd, dd.MM.yyyy"));
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

EinsatzTableWidgetItem *FahrtagWindow::fuegeZeileInTabelleEin()
{
    bool nehmeOld = nehme;
    nehme = false;
    ui->tablePersonen->insertRow(0);

    EinsatzTableWidgetItem *item = new EinsatzTableWidgetItem("");
    ui->tablePersonen->setItem(0, 0, item);

    QComboBox *box = generateNewCategoryComboBox();
    ui->tablePersonen->setCellWidget(0, 1, box);

    QTimeEdit *beginnEdit = generateNewTimeEdit();
    ui->tablePersonen->setCellWidget(0, 2, beginnEdit);

    QTimeEdit *endeEdit = generateNewTimeEdit();
    ui->tablePersonen->setCellWidget(0, 3, endeEdit);

    QTableWidgetItem *bem = new QTableWidgetItem();
    ui->tablePersonen->setItem(0, 4, bem);

    connect(box, &QComboBox::currentTextChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 1); });
    connect(beginnEdit, &QTimeEdit::timeChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 2); });
    connect(endeEdit, &QTimeEdit::timeChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 3); });

    ui->buttonRemovePerson->setEnabled(true);
    nehme = nehmeOld;
    return item;
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
        if (Version::isDeveloperVersion()) {
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
        fahrtag->setPersonalBenoetigt(checked?-1:0);
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
