#include "activitywindow.h"
#include "ui_activitywindow.h"
#include "export.h"

#include <QComboBox>
#include <QMessageBox>
#include <QPrinter>

ActivityWindow::ActivityWindow(QWidget *parent, Activity *a) : QMainWindow(parent), ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
    ui->tablePersonen->resizeColumnsToContents();
    activity = a;
    loadData();
    setWindowTitle("Arbeitseinsatz am "+activity->getDatum().toString("dddd dd. MM. yyyy"));

    namen = new QSet<QString>();
}

ActivityWindow::~ActivityWindow()
{
    delete ui;
}

void ActivityWindow::on_dateDatum_dateChanged(const QDate &date)
{
    activity->setDatum(date);
    setWindowTitle("Arbeitseinsatz am "+date.toString("dddd dd. MM. yyyy"));
}

void ActivityWindow::on_lineOrt_textChanged(const QString &arg1)
{
    activity->setOrt(arg1);
}

void ActivityWindow::on_lineAnlass_textChanged()
{
    activity->setAnlass(ui->lineAnlass->text());
}

void ActivityWindow::on_plainBeschreibung_textChanged()
{
    activity->setBemerkungen(ui->plainBeschreibung->toPlainText());
}

void ActivityWindow::on_timeBeginn_timeChanged(const QTime &time)
{
    activity->setZeitAnfang(time);
}

void ActivityWindow::on_timeEnde_timeChanged(const QTime &time)
{
    activity->setZeitEnde(time);
}

void ActivityWindow::on_checkBoxBenoetigt_toggled(bool checked)
{
    activity->setPersonalBenoetigt(checked);
}

void ActivityWindow::on_buttonInsert_clicked()
{
    ui->tablePersonen->insertRow(0);

    QComboBox *box = activity->generateNewCategoryComboBox();
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
        namen->remove(n);
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
            QSet<QString> *neu = new QSet<QString>();
            for( int i = 1; i <= ui->tablePersonen->rowCount(); i++) {
                QString n = (ui->tablePersonen->item(i, 0) == nullptr) ? "" : ui->tablePersonen->item(i, 0)->text();
                neu->insert(n);
                if (namen->contains(n)) {
                    namen->remove(n);
                }
            }

            if (namen->size() == 1) activity->removePerson(namen->values().at(0));
            namen = neu;
        }

        QString name = (ui->tablePersonen->item(row,0) == nullptr) ? "" : ui->tablePersonen->item(row,0)->text();
        QString aufgabe = ((QComboBox*)ui->tablePersonen->cellWidget(row, 1))->currentText();
        QTime beginnZ = ((QTimeEdit*)ui->tablePersonen->cellWidget(row, 2))->time();
        QTime endeZ = ((QTimeEdit*)ui->tablePersonen->cellWidget(row, 3))->time();
        QString bemerkung = (ui->tablePersonen->item(row, 4) == nullptr) ? "" :  ui->tablePersonen->item(row,4)->text();

        switch (activity->addPerson(name, bemerkung, beginnZ, endeZ, aufgabe)) {
        case Misstake::OK:
            break;
        case Misstake::PersonNichtGefunden:
            QMessageBox::warning(this, tr("Fehler"), tr("Die eingegebene Person konnte im System nicht gefunden werden."));
            break;
        case Misstake::FalscheQualifikation:
            QMessageBox::warning(this, tr("Fehlene Qualifikation"), tr("Die Aufgabe kann/darf nicht von der angegebenen Person übernommen werden, da dies eine Aufgabe ist, welche eine Ausbildung voraussetzt."));
            break;
        default:
            QMessageBox::warning(this, tr("Sonstiger Fehler"), tr("Während der Verarbeitung der Eingabe ist ein Fehler unterlaufen.\nPrüfen Sie Ihre Eingaben und versuchen es erneut!"));
            break;
        }

        nehme = true;
    }
}

void ActivityWindow::on_actionDelete_triggered()
{

}

void ActivityWindow::on_actionPrint_triggered()
{
    QPrinter *p = Export::getPrinterPaper(this);
    Export::printActivity(activity, nullptr, p);
}

void ActivityWindow::on_actionPdf_triggered()
{
    QPrinter *p = Export::getPrinterPDF(this, windowTitle()+".pdf");
    Export::printActivity(activity, p);
}

void ActivityWindow::comboInTableChanged()
{
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
     if (combo)
         on_tablePersonen_cellChanged(combo->property("row").toInt(), combo->property("column").toInt());
}

void ActivityWindow::timeEditInTableChanged()
{
    QTimeEdit *time = qobject_cast<QTimeEdit*>(sender());
     if (time)
         on_tablePersonen_cellChanged(time->property("row").toInt(), time->property("column").toInt());
}

void ActivityWindow::loadData()
{
    nehme = false;
    // Allgemeine Daten von AActivity
    ui->dateDatum->setDate(activity->getDatum());
    ui->lineOrt->setText(activity->getOrt());
    ui->lineAnlass->setText(activity->getAnlass());
    ui->plainBeschreibung->setPlainText(activity->getBemerkungen());
    ui->timeBeginn->setTime(activity->getZeitAnfang());
    ui->timeEnde->setTime(activity->getZeitEnde());
    ui->checkBoxBenoetigt->setChecked(activity->getPersonalBenoetigt());

    // Tabelle laden und alles einfügen
    namen = new QSet<QString>();

    for(Person *p: activity->getPersonen()->keys()) {
        on_buttonInsert_clicked();

        namen->insert(p->getName());

        AActivity::Infos *info = activity->getPersonen()->value(p);

        ui->tablePersonen->setItem(0, 0, new QTableWidgetItem(p->getName()));
        ((QComboBox*)ui->tablePersonen->cellWidget(0, 1))->setCurrentText(AActivity::getStringFromCategory(info->kategorie));
        ((QTimeEdit*)ui->tablePersonen->cellWidget(0, 2))->setTime(info->beginn);
        ((QTimeEdit*)ui->tablePersonen->cellWidget(0, 3))->setTime(info->ende);
        ui->tablePersonen->setItem(0, 4, new QTableWidgetItem(info->bemerkung));
    }
    nehme = true;
}
