#include "activitywindow.h"
#include "ui_activitywindow.h"
#include "export.h"

#include <QMessageBox>
#include <QPrinter>

ActivityWindow::ActivityWindow(QWidget *parent, Activity *a) : QMainWindow(parent), ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
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
    ui->buttonRemove->setEnabled(true);
}

void ActivityWindow::on_buttonRemove_clicked()
{
    int i = ui->tablePersonen->currentRow();
    if (i == -1) return;
    QString n = "";
    if (ui->tablePersonen->item(i, 0) != nullptr) {
        n = ui->tablePersonen->item(i, 0)->text();
    }
    if (activity->removePerson(n)) {
        namen->remove(n);
    }
    ui->tablePersonen->removeRow(i);
    ui->buttonRemove->setEnabled(ui->tablePersonen->rowCount() > 0);
}

void ActivityWindow::on_tablePersonen_cellChanged(int row, int column)
{
    if (nehme) {
        nehme = false;
        // column 1: Name, 2: Beginn, 3: Ende, 4: Aufgabe
        // wenn name geändert wurde, muss der Index über die namen neu aufgebaut werden, da es sonst probleme gibt
        if (column == 0) {
            QStringList *neu = new QStringList();
            for(int i = 1; i <= ui->tablePersonen->rowCount(); i++) {
                QString n = "";
                if (ui->tablePersonen->item(i, 0) != nullptr)
                    n = ui->tablePersonen->item(i, 0)->text();
                if (n.contains(";")) {
                    QStringList splitted = n.split(QRegExp("\\s*;\\s*"));
                    n = splitted.at(0);
                }
                neu->append(n);
                if (namen->contains(n)) {
                    namen->remove(n);
                }
            }
            if (namen->size() == 1) {
                activity->removePerson(namen->values().at(0));
            }
        }

        QString name = "";
        if (ui->tablePersonen->item(row,0) != nullptr)
            name = ui->tablePersonen->item(row,0)->text();

        QString bemerkung = "";
        QStringList list = name.split(QRegExp("\\s*;\\s*"));
        name = list.at(0);
        if (list.length() > 1)
            bemerkung = list.at(1);

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

        if (bemerkung.toUpper().contains("EXTERN")) {
            Person *p = new Person(name);
            p->setAusbildungTf(true);
            p->setAusbildungZf(true);
            p->setAusbildungRangierer(true);
            activity->addPerson(p, bemerkung, beginnZ, endeZ, aufgabe);
        } else {
            Misstake antw = activity->addPerson(name, bemerkung, beginnZ, endeZ, aufgabe);

            switch (antw) {
            case Misstake::OK:
                break;
            case Misstake::PersonNichtGefunden:
                QMessageBox::warning(this, "Fehler", "Die eingegebene Person konnte im System nicht gefunden werden.");
                break;
            case Misstake::FalscheQualifikation:
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
        AActivity::Infos *info = activity->getPersonen()->value(p);

        QListWidgetItem *item;
        if (info->bemerkung == "") {
            item = new QListWidgetItem(p->getName());
        } else {
            item = new QListWidgetItem(p->getName()+"; "+info->bemerkung);
        }
        namen->insert(p->getName());
        ui->buttonRemove->setEnabled(true);

        // Zeile für die Person in die Tabelle einfügen
        QString bem = info->bemerkung.toUpper();

        ui->tablePersonen->insertRow(0);

        QTableWidgetItem *zelleName = new QTableWidgetItem(p->getName());
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
        ui->tablePersonen->setItem(0, 3, zelleAufgabe);
    }
    nehme = true;
}
