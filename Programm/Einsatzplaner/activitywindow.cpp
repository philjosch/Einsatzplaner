#include "activitywindow.h"
#include "ui_activitywindow.h"
#include "export.h"
#include "guihelper.h"

#include <QMessageBox>
#include <eplexception.h>

ActivityWindow::ActivityWindow(QWidget *parent, Activity *a) : QMainWindow(parent), ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
    ui->tablePersonen->resizeColumnsToContents();
    activity = a;

    tabelleZuEinsatz = QMap<QTableWidgetItem*,AActivity::Einsatz>();

    nehme = false;
    // Allgemeine Daten von AActivity
    ui->dateDatum->setDate(activity->getDatum());
    ui->lineOrt->setText(activity->getOrt());
    ui->lineAnlass->setText(activity->getAnlass().replace("<br/>","\n"));
    setPredefinedValue(activity->getAnlass().replace("<br/>","\n"));
    ui->plainBeschreibung->setPlainText(activity->getBemerkungen().replace("<br/>","\n"));
    ui->timeBeginn->setTime(activity->getZeitAnfang());
    ui->timeEnde->setTime(activity->getZeitEnde());
    ui->checkZeiten->setChecked(activity->getZeitenUnbekannt());
    on_checkZeiten_clicked(activity->getZeitenUnbekannt());
    ui->checkBoxBenoetigt->setChecked(activity->getPersonalBenoetigt());

    QMap<AActivity::Einsatz, Infos> personen = activity->getPersonen();
    // Tabelle laden und alles einfügen
    for(AActivity::Einsatz e: personen.keys()) {
        on_buttonInsert_clicked();

        Infos info = personen.value(e);

        ui->tablePersonen->item(0, 0)->setText((e.person)->getName());
        tabelleZuEinsatz.insert(ui->tablePersonen->item(0, 0), e);

        Category kat = info.kategorie;
        if (kat == Category::Begleiter)
            kat = Category::Zub;
        static_cast<QComboBox*>(ui->tablePersonen->cellWidget(0, 1))->setCurrentText(getLocalizedStringFromCategory(kat));
        static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(0, 2))->setTime(info.beginn);
        static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(0, 3))->setTime(info.ende);
        ui->tablePersonen->setItem(0, 4, new QTableWidgetItem(info.bemerkung));
    }

    updateWindowTitle();
    nehme = true;
}

ActivityWindow::~ActivityWindow()
{
    delete ui;
}

void ActivityWindow::on_dateDatum_dateChanged(const QDate &date)
{
    if (nehme) {
        activity->setDatum(date);
    }
    updateWindowTitle();
}

void ActivityWindow::on_lineOrt_textChanged(const QString &arg1)
{
    if (nehme)
        activity->setOrt(arg1);
}

void ActivityWindow::on_lineAnlass_textChanged(const QString &arg1)
{
    QString n = arg1;
    n = n.replace("\n","<br/>");
    if (nehme)
        activity->setAnlass(n);
    setPredefinedValue(n);
    updateWindowTitle();
}

void ActivityWindow::on_plainBeschreibung_textChanged()
{
    if (nehme)
        activity->setBemerkungen(ui->plainBeschreibung->toPlainText().replace("\n","<br/>"));
}

void ActivityWindow::on_timeBeginn_timeChanged(const QTime &time)
{
    if (nehme)
        activity->setZeitAnfang(time);
}
void ActivityWindow::on_timeEnde_timeChanged(const QTime &time)
{
    if (nehme)
        activity->setZeitEnde(time);
}
void ActivityWindow::on_checkZeiten_clicked(bool checked)
{
    ui->timeBeginn->setEnabled(!checked);
    ui->timeEnde->setEnabled(!checked);
    if (nehme)
        activity->setZeitenUnbekannt(checked);
}

void ActivityWindow::on_checkBoxBenoetigt_toggled(bool checked)
{
    if (nehme)
        activity->setPersonalBenoetigt(checked);
}

void ActivityWindow::on_buttonInsert_clicked()
{
    bool nehmeOld = nehme;
    nehme = false;
    ui->tablePersonen->setSortingEnabled(false);
    ui->tablePersonen->insertRow(0);

    QTableWidgetItem *item = new QTableWidgetItem("");
    ui->tablePersonen->setItem(0, 0, item);

    QComboBox *box = generateNewCategoryComboBox();
    box->setCurrentText(getLocalizedStringFromCategory(predefinedValueForTable));
    ui->tablePersonen->setCellWidget(0, 1, box);

    QTimeEdit *beginn = generateNewTimeEdit();
    ui->tablePersonen->setCellWidget(0, 2, beginn);

    QTimeEdit *ende = generateNewTimeEdit();
    ui->tablePersonen->setCellWidget(0, 3, ende);

    ui->tablePersonen->setItem(0, 4, new QTableWidgetItem(""));

    connect(box, &QComboBox::currentTextChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 1); });
    connect(beginn, &QTimeEdit::timeChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 2); });
    connect(ende, &QTimeEdit::timeChanged, this, [=]() { if (nehme) on_tablePersonen_cellChanged(ui->tablePersonen->row(item), 3); });

    tabelleZuEinsatz.insert(item, AActivity::Einsatz{nullptr, Sonstiges});

    ui->buttonRemove->setEnabled(true);
    ui->tablePersonen->repaint();

    ui->tablePersonen->setSortingEnabled(true);
    nehme = nehmeOld;
}

void ActivityWindow::on_buttonRemove_clicked()
{
    int i = ui->tablePersonen->currentRow();
    if (i < 0) return;
    if (tabelleZuEinsatz.contains(ui->tablePersonen->item(i, 0))) {
        AActivity::Einsatz e = tabelleZuEinsatz.value(ui->tablePersonen->item(i, 0));
        activity->removePerson(e.person, e.cat);
    }

    tabelleZuEinsatz.remove(ui->tablePersonen->item(i, 0));
    ui->tablePersonen->removeRow(i);

    ui->buttonRemove->setEnabled(ui->tablePersonen->rowCount() > 0);
}

void ActivityWindow::on_tablePersonen_cellChanged(int row, int column)
{
    // column 0: Name | 1: Aufgabe | 2: Beginn | 3: Ende | 4: Bemerkung
    // Wenn der Name geändert wurde, muss die Verknuepfung mit der alten Person aufgeloest werden
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
            activity->updatePersonInfos(e.person, e.cat, neu);
        } else if (column == 4) {
            activity->updatePersonBemerkung(e.person, e.cat, bemerkung);
        }
        if (neuHinzufuegen) {
            if (e.person != nullptr)
                activity->removePerson(e.person, e.cat);

            try {
                AActivity::Einsatz e = activity->addPerson(name, bemerkung, beginn, ende, kat);
                tabelleZuEinsatz.insert(item, e);
                if (! e.person->getAktiv()) {
                    QMessageBox::information(this, tr("Information"), tr("Die Person wird als passives Mitglied geführt. Sie wurde aber dennoch eingetragen!"));
                }
            }  catch (AActivityException &e) {
                QMessageBox::warning(this, tr("Fehler"), e.getError());
            }
        }
        nehme = true;
    }
}

void ActivityWindow::on_actionPrint_triggered()
{
    Export::Aktivitaeten::printAktivitaetenEinzel({activity},
                                    Export::getPrinterPaper(this, QPrinter::Orientation::Portrait));
}
void ActivityWindow::on_actionPdf_triggered()
{
    Export::Aktivitaeten::printAktivitaetenEinzel({activity},
                                    Export::getPrinterPDF(this, windowTitle()+".pdf", QPrinter::Orientation::Portrait));
}

void ActivityWindow::on_buttonDelete_clicked()
{
    if (QMessageBox::question(this, tr("Wirklich löschen?"), tr("Möchten Sie diesen Arbeitseinsatz wirklich unwiderruflich löschen?")) == QMessageBox::Yes) {
        emit activity->del(activity);
        this->close();
        deleteLater();
    }
}

void ActivityWindow::setPredefinedValue(QString anlass)
{
    if (anlass.contains(tr("Vorbereiten"), Qt::CaseInsensitive)
            || anlass.contains(tr("Vorbereitung"), Qt::CaseInsensitive)
            || anlass.contains(tr("Reinig"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::ZugVorbereiten;
    } else if (anlass.contains(tr("Werkstatt"), Qt::CaseInsensitive)) {
        predefinedValueForTable = Category::Werkstatt;
    } else if (anlass.contains(tr("Ausbildung"), Qt::CaseInsensitive)) {
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
    setWindowTitle(tr("%1 am %2").arg(activity->getKurzbeschreibung()).arg(activity->getDatum().toString("dddd dd.MM. yyyy")));
}
