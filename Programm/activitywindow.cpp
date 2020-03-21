#include "activitywindow.h"
#include "ui_activitywindow.h"
#include "export.h"

#include <QMessageBox>

ActivityWindow::ActivityWindow(QWidget *parent, Activity *a) : QMainWindow(parent), ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
    ui->tablePersonen->resizeColumnsToContents();
    activity = a;

    namen = QSet<QString>();
    predefinedValueForTable = Category::Sonstiges;

    loadData();
    setWindowTitle(tr("%1 am %2").arg(activity->getKurzbeschreibung()).arg(activity->getDatum().toString("dddd dd.MM. yyyy")));
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
    setWindowTitle(tr("%1 am %2").arg(activity->getKurzbeschreibung()).arg(activity->getDatum().toString("dddd dd.MM. yyyy")));
}

void ActivityWindow::on_lineOrt_textChanged(const QString &arg1)
{
    if (nehme)
        activity->setOrt(arg1);
}

void ActivityWindow::on_lineAnlass_textChanged(const QString &arg1)
{
    if (nehme)
        activity->setAnlass(arg1);
    setPredefinedValue(arg1);
    setWindowTitle(tr("%1 am %2").arg(activity->getKurzbeschreibung()).arg(activity->getDatum().toString("dddd dd.MM. yyyy")));
}

void ActivityWindow::on_plainBeschreibung_textChanged()
{
    if (nehme)
        activity->setBemerkungen(ui->plainBeschreibung->toPlainText());
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
    ui->tablePersonen->insertRow(0);

    QComboBox *box = activity->generateNewCategoryComboBox();
    box->setCurrentText(AActivity::getStringFromCategory(predefinedValueForTable));
    connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(comboInTableChanged()));
    ui->tablePersonen->setCellWidget(0, 1, box);

    QTimeEdit *beginn = activity->generateNewTimeEdit();
    connect(beginn, SIGNAL(timeChanged(QTime)), this, SLOT(timeEditInTableChanged()));
    ui->tablePersonen->setCellWidget(0, 2, beginn);

    QTimeEdit *ende = activity->generateNewTimeEdit();
    connect(ende, SIGNAL(timeChanged(QTime)), this, SLOT(timeEditInTableChanged()));
    ui->tablePersonen->setCellWidget(0, 3, ende);

    ui->buttonRemove->setEnabled(true);
}

void ActivityWindow::on_buttonRemove_clicked()
{
    int i = ui->tablePersonen->currentRow();
    if (i == -1) return;
    QString n = (ui->tablePersonen->item(i, 0) == nullptr) ? "" : ui->tablePersonen->item(i, 0)->text();
    if (activity->removePerson(n)) {
        namen.remove(n);
    }
    ui->tablePersonen->removeRow(i);
    ui->buttonRemove->setEnabled(ui->tablePersonen->rowCount() > 0);
}

void ActivityWindow::on_tablePersonen_cellChanged(int row, int column)
{
    ui->tablePersonen->resizeColumnsToContents();
    if (nehme) {
        nehme = false;
        // column 0: Name, 1: Aufgabe, 2: Beginn, 3: Ende, 4: Bemerkung
        // wenn name geändert wurde, muss der Index über die namen neu aufgebaut werden, da es sonst probleme gibt
        if (column == 0) {
            QSet<QString> neu = QSet<QString>();
            for( int i = 0; i < ui->tablePersonen->rowCount(); i++) {
                QString n = (ui->tablePersonen->item(i, 0) == nullptr) ? "" : ui->tablePersonen->item(i, 0)->text();
                neu.insert(n);
                if (namen.contains(n)) {
                    namen.remove(n);
                }
            }

            if (namen.size() == 1) activity->removePerson(namen.values().at(0));
            namen = neu;
        }

        QString name = (ui->tablePersonen->item(row,0) == nullptr) ? "" : ui->tablePersonen->item(row,0)->text();
        Category kat = AActivity::getCategoryFromString(static_cast<QComboBox*>(ui->tablePersonen->cellWidget(row, 1))->currentText());
        QTime beginnZ = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 2))->time();
        QTime endeZ = static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(row, 3))->time();
        QString bemerkung = (ui->tablePersonen->item(row, 4) == nullptr) ? "" :  ui->tablePersonen->item(row,4)->text();
        if (activity->isExtern(bemerkung)) activity->removePerson(name);

        switch (activity->addPerson(name, bemerkung, beginnZ, endeZ, kat)) {
        case Mistake::OK:
        case Mistake::ExternOk:
            ui->tablePersonen->sortItems(0);
            ui->tablePersonen->setSortingEnabled(false);
            break;
        case Mistake::PersonNichtGefunden:
            QMessageBox::warning(this, tr("Fehler"), tr("Die eingegebene Person konnte im System nicht gefunden werden."));
            break;
        case Mistake::FalscheQualifikation:
            QMessageBox::warning(this, tr("Fehlene Qualifikation"), tr("Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt."));
            break;
        default:
            QMessageBox::warning(this, tr("Sonstiger Fehler"), tr("Während der Verarbeitung der Eingabe ist ein Fehler unterlaufen.\nPrüfen Sie Ihre Eingaben und versuchen es erneut!"));
            break;
        }

        nehme = true;
    }
}
void ActivityWindow::comboInTableChanged()
{
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (combo) {
        int row = -1;
        for(int i = 0; i < ui->tablePersonen->rowCount(); i++) {
            if (ui->tablePersonen->cellWidget(i, 1) == combo) {
                row = i;
                break;
            }
        }
        if (row >= 0)
            on_tablePersonen_cellChanged(row, 1);
    }
}
void ActivityWindow::timeEditInTableChanged()
{
    QTimeEdit *time = qobject_cast<QTimeEdit*>(sender());
    if (time) {
        int row = -1;
        int column = -1;
        for(int i = 0; i < ui->tablePersonen->rowCount(); i++) {
            if (ui->tablePersonen->cellWidget(i, 2) == time) {
                row = i;
                column = 2;
                break;
            }
            if (ui->tablePersonen->cellWidget(i, 3) == time) {
                row = i;
                column = 3;
                break;
            }
        }
        if (row >= 0 && column >= 0)
            on_tablePersonen_cellChanged(row, column);
    }
}

void ActivityWindow::on_actionDelete_triggered()
{
    on_buttonDelete_clicked();
}

void ActivityWindow::on_actionPrint_triggered()
{
    QPrinter *p = Export::getPrinterPaper(this);
    Export::print(activity, p);
}

void ActivityWindow::on_actionPdf_triggered()
{
    QPrinter *p = Export::getPrinterPDF(this, windowTitle()+".pdf");
    Export::print(activity, p);
}

void ActivityWindow::on_buttonDelete_clicked()
{
    if (QMessageBox::question(this, tr("Wirklich löschen?"), tr("Möchten Sie diesen Arbeitseinsatz wirklich unwiderruflich löschen?")) == QMessageBox::Yes) {
        activity->deletter();
        this->close();
        deleteLater();
    }
}

void ActivityWindow::loadData()
{
    nehme = false;
    // Allgemeine Daten von AActivity
    ui->dateDatum->setDate(activity->getDatum());
    ui->lineOrt->setText(activity->getOrt());
    ui->lineAnlass->setText(activity->getAnlass());
    setPredefinedValue(activity->getAnlass());
    ui->plainBeschreibung->setPlainText(activity->getBemerkungen());
    ui->timeBeginn->setTime(activity->getZeitAnfang());
    ui->timeEnde->setTime(activity->getZeitEnde());
    ui->checkZeiten->setChecked(activity->getZeitenUnbekannt());
    on_checkZeiten_clicked(activity->getZeitenUnbekannt());
    ui->checkBoxBenoetigt->setChecked(activity->getPersonalBenoetigt());

    // Tabelle laden und alles einfügen
    namen = QSet<QString>();

    for(Person *p: activity->getPersonen().keys()) {
        on_buttonInsert_clicked();

        namen.insert(p->getName());

        AActivity::Infos info = *(activity->getPersonen().value(p));

        ui->tablePersonen->setItem(0, 0, new QTableWidgetItem(p->getName()));
        Category kat = info.kategorie;
        if (kat == Category::Begleiter)
            kat = Category::Zub;
        static_cast<QComboBox*>(ui->tablePersonen->cellWidget(0, 1))->setCurrentText(AActivity::getStringFromCategory(kat));
        static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(0, 2))->setTime(info.beginn);
        static_cast<QTimeEdit*>(ui->tablePersonen->cellWidget(0, 3))->setTime(info.ende);
        ui->tablePersonen->setItem(0, 4, new QTableWidgetItem(info.bemerkung));
    }
    nehme = true;
}

void ActivityWindow::setPredefinedValue(QString anlass)
{
    anlass = anlass.replace(" ", "").toLower();
    if (anlass.contains(tr("Vorbereiten").toLower()) || anlass.contains(tr("Vorbereitung").toLower())) {
        predefinedValueForTable = Category::ZugVorbereiten;
    } else if (anlass.contains(tr("Werkstatt").toLower())) {
        predefinedValueForTable = Category::Werkstatt;
    } else if (anlass.contains(tr("Ausbildung").toLower())) {
        predefinedValueForTable = Category::Ausbildung;
    } else if (anlass.contains(tr("Büro").toLower())) {
        predefinedValueForTable = Category::Buero;
    } else {
        predefinedValueForTable = Category::Sonstiges;
    }
}
