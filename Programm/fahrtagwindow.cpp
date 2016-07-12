#include "fahrtagwindow.h"
#include "ui_fahrtagwindow.h"

FahrtagWindow::FahrtagWindow(QWidget *parent, Fahrtag *f) :
    QMainWindow(parent),
    ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
    fahrtag = f;
    loadData();
    ui->dateDate->setFocus();
}

FahrtagWindow::~FahrtagWindow()
{
    delete ui;
}

void FahrtagWindow::on_dateDate_dateChanged(const QDate &date)
{
    fahrtag->setDatum(new QDate(date.year(), date.month(), date.day()));
}

void FahrtagWindow::on_comboArt_currentIndexChanged(int index)
{
    fahrtag->setArt((Fahrtag::Art)index);
}

void FahrtagWindow::on_textAnlass_textChanged()
{
    fahrtag->setAnlass(ui->textAnlass->toPlainText());
}

void FahrtagWindow::on_checkWichtig_stateChanged(int arg1)
{

}

void FahrtagWindow::on_comboWagenreihung_currentTextChanged(const QString &arg1)
{

}

void FahrtagWindow::on_comboTimeTfH_currentTextChanged(const QString &arg1)
{

}

void FahrtagWindow::on_comboTimeTfM_currentTextChanged(const QString &arg1)
{

}

void FahrtagWindow::on_comboTimeZH_currentTextChanged(const QString &arg1)
{

}

void FahrtagWindow::on_comboTimeZM_currentTextChanged(const QString &arg1)
{

}

void FahrtagWindow::on_listTf_itemChanged(QListWidgetItem *item)
{

}

void FahrtagWindow::on_listZub_itemChanged(QListWidgetItem *item)
{

}

void FahrtagWindow::on_listZf_itemChanged(QListWidgetItem *item)
{

}

void FahrtagWindow::on_listService_itemChanged(QListWidgetItem *item)
{

}

void FahrtagWindow::on_textBemerkungen_textChanged()
{
    fahrtag->setBemerkungen(ui->textBemerkungen->toPlainText());
}

void FahrtagWindow::on_plainBeschreibung_textChanged()
{

}

void FahrtagWindow::on_timeBeginn_timeChanged(const QTime &time)
{
    fahrtag->setZeitAnfang(new QTime(time.hour(), time.minute()));
}

void FahrtagWindow::on_timeEnde_timeChanged(const QTime &time)
{
    fahrtag->setZeitEnde(new QTime(time.hour(), time.minute()));
}

void FahrtagWindow::on_checkBoxBenoetigt_stateChanged(int arg1)
{
    fahrtag->setPersonalBenoetigt(ui->checkBoxBenoetigt->isChecked());
}

void FahrtagWindow::loadData()
{
    ui->dateDate->setDate(*fahrtag->getDatum());
    ui->textAnlass->clear();
    ui->textAnlass->insertPlainText(fahrtag->getAnlass());
}
