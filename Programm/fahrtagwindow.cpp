#include "fahrtagwindow.h"
#include "ui_fahrtagwindow.h"

FahrtagWindow::FahrtagWindow(QWidget *parent, Fahrtag *f) :
    QMainWindow(parent),
    ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
    ui->frameReservierungen = f;
    fahrtag = f;
    loadData();
    ui->dateDate->setFocus();
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum()->toString("dddd dd. MM. yyyy"));
}

FahrtagWindow::~FahrtagWindow()
{
    delete ui;
}

void FahrtagWindow::on_dateDate_dateChanged(const QDate &date)
{
    fahrtag->setDatum(new QDate(date.year(), date.month(), date.day()));
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum()->toString("dddd dd. MM. yyyy"));
}

void FahrtagWindow::on_comboArt_currentIndexChanged(int index)
{
    fahrtag->setArt((Fahrtag::Art)index);
    setWindowTitle(Fahrtag::getStringFromArt(fahrtag->getArt())+" am "+fahrtag->getDatum()->toString("dddd dd. MM. yyyy"));
}

void FahrtagWindow::on_textAnlass_textChanged()
{
    fahrtag->setAnlass(ui->textAnlass->toPlainText());
}

void FahrtagWindow::on_checkWichtig_stateChanged(int arg1)
{
    fahrtag->setWichtig(arg1 == 1);
}

void FahrtagWindow::on_comboWagenreihung_currentTextChanged(const QString &arg1)
{
    fahrtag->setWagenreihung(arg1);
}

void FahrtagWindow::on_comboTimeTfH_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitTf(new QTime(arg1.toInt(), fahrtag->getZeitTf()->minute()));
}

void FahrtagWindow::on_comboTimeTfM_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitTf(new QTime(fahrtag->getZeitTf()->minute(), arg1.toInt()));
}

void FahrtagWindow::on_comboTimeZH_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitAnfang(new QTime(arg1.toInt(), fahrtag->getZeitAnfang()->minute()));
}

void FahrtagWindow::on_comboTimeZM_currentTextChanged(const QString &arg1)
{
    fahrtag->setZeitAnfang(new QTime(fahrtag->getZeitAnfang()->minute(), arg1.toInt()));
}

void FahrtagWindow::on_listTf_itemChanged(QListWidgetItem *item)
{
    // alle daten in eine Liste laden und dann
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
// Dekativiert, wird im Moment nicht benoetigt
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
    fahrtag->setPersonalBenoetigt(arg1 == 1);
}

void FahrtagWindow::loadData()
{
    // Allgemeine Daten von AActivity
    ui->dateDate->setDate(*fahrtag->getDatum());
    ui->textAnlass->clear();
    ui->textAnlass->insertPlainText(fahrtag->getAnlass());
    ui->timeEnde->setTime(* fahrtag->getZeitEnde());
    ui->checkBoxBenoetigt->setChecked(fahrtag->getPersonalBenoetigt());

    // Daten von ManagerReservierungen
    ui->comboWagenreihung->setCurrentText(fahrtag->getWagenreihung());

    // Daten von Fahrtag
    /* LIstTf, ListZf, ListZub, ListService fehlt noch */
    ui->comboArt->setCurrentIndex((int)fahrtag->getArt());
    ui->comboTimeTfH->setCurrentText(fahrtag->getZeitTf()->toString("HH"));
    ui->comboTimeTfM->setCurrentText(fahrtag->getZeitTf()->toString("mm"));
    ui->comboTimeZH->setCurrentText(fahrtag->getZeitAnfang()->toString("HH"));
    ui->comboTimeZM->setCurrentText(fahrtag->getZeitAnfang()->toString("mm"));
    ui->checkWichtig->setChecked(fahrtag->getWichtig());
    ui->checkTf->setChecked(fahrtag->getBenoetigeTf());
    ui->checkZf->setChecked(fahrtag->getBenoetigeZf());
    ui->checkZub->setChecked(fahrtag->getBenoetigeZub());
    ui->checkService->setChecked(fahrtag->getBenoetigeService());

}

void FahrtagWindow::addItemTolist(QListWidget *l, QPushButton *b)
{
    l->addItem("(Name eingeben)");
    l->item(l->count()-1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
    b->setEnabled(true);
}
void FahrtagWindow::deleteItemFromList(QListWidget *l, QPushButton *b)
{
    l->takeItem(l->currentRow());
    b->setEnabled(l->count()>0);
}

void FahrtagWindow::on_buttonTfAdd_clicked()
{
    addItemTolist(ui->listTf, ui->buttonTfDelete);
}
void FahrtagWindow::on_buttonTfDelete_clicked()
{
    deleteItemFromList(ui->listTf, ui->buttonTfDelete);
}

void FahrtagWindow::on_buttonZfAdd_clicked()
{
    addItemTolist(ui->listZf, ui->buttonZfDelete);
}
void FahrtagWindow::on_buttonZfDelete_clicked()
{
    deleteItemFromList(ui->listZf, ui->buttonZfDelete);
}

void FahrtagWindow::on_buttonZubAdd_clicked()
{
    addItemTolist(ui->listZub, ui->buttonZubDelete);
}
void FahrtagWindow::on_buttonZubDelete_clicked()
{
    deleteItemFromList(ui->listZub, ui->buttonZubDelete);
}

void FahrtagWindow::on_buttonServiceAdd_clicked()
{
    addItemTolist(ui->listService, ui->buttonServiceDelete);
}
void FahrtagWindow::on_buttonServiceDelete_clicked()
{
    deleteItemFromList(ui->listService, ui->buttonServiceDelete);
}
