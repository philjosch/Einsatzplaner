#include "activitywindow.h"
#include "ui_activitywindow.h"

ActivityWindow::ActivityWindow(QWidget *parent, Activity *a) :
    QMainWindow(parent),
    ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
    activity = a;
    loadData();
}

ActivityWindow::~ActivityWindow()
{
    delete ui;
}

void ActivityWindow::on_buttonInsert_clicked()
{
    ui->tablePersonen->insertRow(ui->tablePersonen->currentRow());
}

void ActivityWindow::on_buttonRemove_clicked()
{
    ui->tablePersonen->removeRow(ui->tablePersonen->currentRow());
}

void ActivityWindow::on_dateDatum_dateChanged(const QDate &date)
{
    activity->setDatum(new QDate(date.year(), date.month(), date.day()));
}

void ActivityWindow::on_lineOrt_textChanged(const QString &arg1)
{
    activity->setOrt(arg1);
}

void ActivityWindow::on_plainAnlass_textChanged()
{
    activity->setAnlass(ui->plainAnlass->toPlainText());
}

void ActivityWindow::on_plainBeschreibung_textChanged()
{
    activity->setBemerkungen(ui->plainBeschreibung->toPlainText());
}

void ActivityWindow::on_timeBeginn_timeChanged(const QTime &time)
{
    activity->setZeitAnfang(new QTime(time.hour(), time.minute()));
}

void ActivityWindow::on_timeEnde_timeChanged(const QTime &time)
{
    activity->setZeitEnde(new QTime(time.hour(), time.minute()));
}

void ActivityWindow::on_checkBoxBenoetigt_toggled(bool checked)
{
    activity->setPersonalBenoetigt(checked);
}

void ActivityWindow::on_tablePersonen_cellChanged(int row, int column)
{
    // column 1: Name, 2: Beginn, 3: Ende, 4: Aufgabe
    /*
     * Hier muss etwas aufwendiger gearbeitet werden,
     * denn die veränderte Spalte muss eingefügt werden
     * und die Liste muss up-todate gehalten werden
     * */
}

void ActivityWindow::loadData()
{
    // Allgemeine Daten von AActivity
    ui->dateDatum->setDate(* activity->getDatum());
    ui->lineOrt->setText(activity->getOrt());
    ui->plainAnlass->clear();
    ui->plainAnlass->insertPlainText(activity->getAnlass());
    ui->plainBeschreibung->setPlainText(activity->getBemerkungen());
    ui->timeBeginn->setTime(* activity->getZeitAnfang());
    ui->timeEnde->setTime(* activity->getZeitEnde());
    ui->checkBoxBenoetigt->setChecked(activity->getPersonalBenoetigt());
    // Tabelle laden und alles einfügen
}
