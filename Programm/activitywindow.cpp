#include "activitywindow.h"
#include "ui_activitywindow.h"
#include "export.h"

#include <QMessageBox>
#include <QPrinter>

ActivityWindow::ActivityWindow(QWidget *parent, Activity *a) :
    QMainWindow(parent),
    ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
    activity = a;
    loadData();
    nehme = true;
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
                activity->removePerson(namen->values().at(0));
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
            activity->addPerson(p, "", beginnZ, endeZ, aufgabe);
        } else {
            ManagerPersonal::Misstake antw = activity->addPerson(name, "", beginnZ, endeZ, aufgabe);

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
    // Allgemeine Daten von AActivity
    ui->dateDatum->setDate(activity->getDatum());
    ui->lineOrt->setText(activity->getOrt());
    ui->lineAnlass->setText(activity->getAnlass());
    ui->plainBeschreibung->setPlainText(activity->getBemerkungen());
    ui->timeBeginn->setTime(activity->getZeitAnfang());
    ui->timeEnde->setTime(activity->getZeitEnde());
    ui->checkBoxBenoetigt->setChecked(activity->getPersonalBenoetigt());
    // Tabelle laden und alles einfügen
    // Fehlt //
}
