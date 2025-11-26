#include "activitywindow.h"
#include "ui_activitywindow.h"
#include "export.h"
#include "guihelper.h"
#include "person.h"

#include <QMessageBox>
#include <eplexception.h>

using namespace EplException;

ActivityWindow::ActivityWindow(CoreMainWindow *parent, AActivity *a) : QMainWindow(parent), ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
    ui->tablePersonen->resizeColumnsToContents();
    activity = a;

    connect(this, &ActivityWindow::loeschen, parent, &CoreMainWindow::loeschenAktivitaet);

    connect(ui->buttonInsert, &QPushButton::clicked, this, [=]() { addEinsatzToTable(new Einsatz(predefinedValueForTable)); });
    connect(ui->buttonRemove, &QPushButton::clicked, this, &ActivityWindow::removePerson);

    connect(ui->dateDatum, &QDateEdit::dateChanged, this, &ActivityWindow::changedDate);
    connect(ui->lineOrt, &QLineEdit::textChanged, this, &ActivityWindow::changedLocation);
    connect(ui->comboAnlass, &QComboBox::currentTextChanged, this, &ActivityWindow::changedPurpose);
    connect(ui->plainBeschreibung, &QPlainTextEdit::textChanged, this, &ActivityWindow::changedDescription);

    connect(ui->timeBeginn, &QTimeEdit::timeChanged, this, &ActivityWindow::changedTimeStart);
    connect(ui->timeEnde, &QTimeEdit::timeChanged, this, &ActivityWindow::changedTimeEnd);
    connect(ui->checkZeiten, &QCheckBox::clicked, this, &ActivityWindow::changedTimeStatus);

    connect(ui->checkWichtig, &QCheckBox::clicked, this, &ActivityWindow::changedImportant);
    connect(ui->checkAbgesagt, &QCheckBox::clicked, this, &ActivityWindow::changedCancelled);

    connect(ui->checkBoxBenoetigt, &QCheckBox::toggled, this, &ActivityWindow::changedRequired);

    connect(ui->tablePersonen, &QTableWidget::cellChanged, this, &ActivityWindow::changedTableCell);

    connect(ui->actionPrint, &QAction::triggered, this, &ActivityWindow::exportPrint);
    connect(ui->actionPdf, &QAction::triggered, this, &ActivityWindow::exportPdf);

    connect(ui->actionDelete, &QAction::triggered, this, &ActivityWindow::deleteTriggered);

    nehme = false;
    // Allgemeine Daten von AActivity
    ui->dateDatum->setDate(activity->getDatum());
    ui->lineOrt->setText(activity->getOrt());
    ui->comboAnlass->setCurrentText(activity->getAnlass().replace("<br/>","\n"));
    setPredefinedValue(activity->getAnlass().replace("<br/>","\n"));
    ui->plainBeschreibung->setPlainText(activity->getBemerkungen().replace("<br/>","\n"));
    ui->timeBeginn->setTime(activity->getZeitAnfang());
    ui->timeEnde->setTime(activity->getZeitEnde());
    ui->checkZeiten->setChecked(activity->getZeitenUnbekannt());
    changedTimeStatus(activity->getZeitenUnbekannt());
    ui->checkWichtig->setChecked(activity->getWichtig());
    ui->checkAbgesagt->setChecked(activity->getAbgesagt());
    ui->checkBoxBenoetigt->setChecked(activity->getPersonalBenoetigt());

    // Tabelle laden und alles einfügen
    for(Einsatz *e: activity->getPersonen()) {
        addEinsatzToTable(e);
    }

    updateWindowTitle();
    nehme = true;
}

ActivityWindow::~ActivityWindow()
{
    delete ui;
}

void ActivityWindow::changedDate(const QDate &date)
{
    if (nehme) {
        activity->setDatum(date);
    }
    updateWindowTitle();
}

void ActivityWindow::changedLocation(const QString &arg1)
{
    if (nehme)
        activity->setOrt(arg1);
}

void ActivityWindow::changedPurpose(const QString &arg1)
{
    QString n = arg1;
    n = n.replace("\n","<br/>");
    if (nehme)
        activity->setAnlass(n);
    setPredefinedValue(n);
    updateWindowTitle();
}

void ActivityWindow::changedDescription()
{
    if (nehme)
        activity->setBemerkungen(ui->plainBeschreibung->toPlainText().replace("\n","<br/>"));
}

void ActivityWindow::changedTimeStart(const QTime &time)
{
    if (nehme)
        activity->setZeitAnfang(time);
}
void ActivityWindow::changedTimeEnd(const QTime &time)
{
    if (nehme)
        activity->setZeitEnde(time);
}
void ActivityWindow::changedTimeStatus(bool checked)
{
    ui->timeBeginn->setEnabled(!checked);
    ui->timeEnde->setEnabled(!checked);
    if (nehme)
        activity->setZeitenUnbekannt(checked);
}

void ActivityWindow::changedImportant(bool checked)
{
    if (nehme)
        activity->setWichtig(checked);
}

void ActivityWindow::changedCancelled(bool checked)
{
    if (nehme)
        activity->setAbgesagt(checked);
}

void ActivityWindow::changedRequired(bool checked)
{
    if (nehme)
        activity->setPersonalBenoetigt(checked);
}

void ActivityWindow::addEinsatzToTable(Einsatz *e)
{
    if (e == nullptr)
        return;

    bool nehmeOld = nehme;
    nehme = false;

    ui->tablePersonen->setSortingEnabled(false);

    ui->tablePersonen->insertRow(0);

    EinsatzTableWidgetItem *item = new EinsatzTableWidgetItem(e);
    ui->tablePersonen->setItem(0, 0, item);

    QComboBox *box = generateNewCategoryComboBox();
    box->setCurrentText(
        toString((e->getKategorie() == Begleiter ? Zub : e->getKategorie()))
        );
    ui->tablePersonen->setCellWidget(0, 1, box);
    connect(box, &QComboBox::currentTextChanged, this, [=]() { if (nehme) changedTableCell(ui->tablePersonen->row(item), 1); });

    QTableWidgetItem *beginnItem = new QTableWidgetItem();
    beginnItem->setData(Qt::EditRole, e->getBeginnAbweichend());
    ui->tablePersonen->setItem(0, 2, beginnItem);

    QTableWidgetItem *endeItem = new QTableWidgetItem();
    endeItem->setData(Qt::EditRole, e->getEndeAbweichend());
    ui->tablePersonen->setItem(0, 3, endeItem);

    QTableWidgetItem *bem = new QTableWidgetItem(e->getBemerkung());
    ui->tablePersonen->setItem(0, 4, bem);


    ui->buttonRemove->setEnabled(true);
    ui->tablePersonen->setSortingEnabled(true);
    nehme = nehmeOld;
}

void ActivityWindow::removePerson()
{
    int i = ui->tablePersonen->currentRow();
    if (i < 0) return;
    EinsatzTableWidgetItem *ptwi = static_cast<EinsatzTableWidgetItem*>(ui->tablePersonen->item(i, 0));
    activity->removePerson(ptwi->getEinsatz());

    ui->tablePersonen->removeRow(i);

    ui->buttonRemove->setEnabled(ui->tablePersonen->rowCount() > 0);
}

void ActivityWindow::changedTableCell(int row, [[maybe_unused]] int column)
{
    // column 0: Name | 1: Aufgabe | 2: Beginn | 3: Ende | 4: Bemerkung
    // Wenn der Name geändert wurde, muss die Verknuepfung mit der alten Person aufgeloest werden
    // Dies ist auch erforderlich bei Aenderung der Kategorie oder bemerkung, um eventuelle Kompetenzen zu pruefen.
    if (! nehme) return;
    nehme = false;

    EinsatzTableWidgetItem *ptwi = static_cast<EinsatzTableWidgetItem*>(ui->tablePersonen->item(row, 0));
    QTime beginn = ui->tablePersonen->item(row, 2)->data(Qt::EditRole).toTime();
    QTime ende = ui->tablePersonen->item(row, 3)->data(Qt::EditRole).toTime();

    if (column == 2 || column == 3) {
        ptwi->getEinsatz()->setBeginnAbweichend(beginn);
        ptwi->getEinsatz()->setEndeAbweichend(ende);
        nehme = true;
        return;
    }

    activity->removePerson(ptwi->getEinsatz());

    QString name = ptwi->text();
    Category kat = getCategoryFromLocalizedString(static_cast<QComboBox*>(ui->tablePersonen->cellWidget(row, 1))->currentText());
    QString bemerkung = (ui->tablePersonen->item(row, 4) == nullptr) ? "" :  ui->tablePersonen->item(row,4)->text();

    try {
        Einsatz *e = activity->addPerson(name, bemerkung, kat);
        e->setBeginnAbweichend(beginn);
        e->setEndeAbweichend(ende);
        ptwi->setEinsatz(e);
        if (! e->getPerson()->getAktiv()) {
            QMessageBox::information(this, tr("Information"), tr("Die Person wird als passives Mitglied geführt. Sie wurde aber dennoch eingetragen!"));
        }
    }  catch (AActivityException &e) {
        QMessageBox::warning(this, tr("Fehler"), e.getError());
    }
    nehme = true;
}

void ActivityWindow::exportPrint()
{
    activity->exportAsHtml(Export::getPrinterPaper(this, QPageLayout::Orientation::Portrait));
}
void ActivityWindow::exportPdf()
{
    activity->exportAsHtml(Export::getPrinterPDF(this, windowTitle(), QPageLayout::Orientation::Portrait));
}

void ActivityWindow::deleteTriggered()
{
    emit loeschen(activity);
}

void ActivityWindow::setPredefinedValue(QString anlass)
{
    if (anlass.contains(tr("Vorbereiten"), Qt::CaseInsensitive)
            || anlass.contains(tr("Vorbereitung"), Qt::CaseInsensitive)
            || anlass.contains(tr("Reinig"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::ZugVorbereiten;
    } else if (anlass.contains(tr("Werkstatt"), Qt::CaseInsensitive)
               || anlass.contains(tr("Waggonarbeiten"), Qt::CaseInsensitive)
               || anlass.contains(tr("Innenausbau"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::Werkstatt;
    } else if (anlass.contains(tr("Ausbildung"), Qt::CaseInsensitive)
               || anlass.contains(tr("RFU"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::Ausbildung;
    } else if (anlass.contains(tr("Büro"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::Buero;
    } else if (anlass.contains(tr("Vegetation"), Qt::CaseInsensitive)
               || anlass.contains(tr("Kontrollfahrt"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::Infrastruktur;
    } else {
        predefinedValueForTable = Category::Sonstiges;
    }
}

void ActivityWindow::updateWindowTitle()
{
    setWindowTitle(tr("%1 am %2").arg(activity->getStringShort(), QLocale().toString(activity->getDatum(), "dddd, dd.MM.yyyy")));
}
