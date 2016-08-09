#include "managerreservierungen.h"
#include "ui_managerreservierungen.h"

#include <QMessageBox>

ManagerReservierungen::ManagerReservierungen(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ManagerReservierungen)
{
    ui->setupUi(this);
    ui->lineSitze->setValidator(new QRegExpValidator(QRegExp("2[0-9]{2}\\s*\\:\\s*(((([1-9][0-9]?)\\s*-\\s*([1-9][0-9]?))|([1-9][0-9]?)))(\\s*,\\s*((([1-9][0-9]?)-([1-9][0-9]?))|([1-9][0-9]?)))*(;\\s*[1-9][0-9]?\\s*\\:\\s*(((([1-9][0-9]?)\\s*-\\s*([1-9][0-9]?))|([1-9][0-9]?)))(\\s*,\\s*((([1-9][0-9]?)-([1-9][0-9]?))|([1-9][0-9]?)))*\\s*)*")));
    wagenreihung = "309, 221, 204, 208";
    nehme = false;
    reservierungen = new QSet<Reservierung*>();
    resToItem = new QMap<Reservierung*, QListWidgetItem*>();
    itemToRes = new QMap<QListWidgetItem*, Reservierung*>();
}

ManagerReservierungen::~ManagerReservierungen()
{
    delete ui;
}

QString ManagerReservierungen::getWagenreihung() const
{
    return wagenreihung;
}

void ManagerReservierungen::setWagenreihung(const QString &value)
{
    wagenreihung = value;
}

QString ManagerReservierungen::getStringFromPlaetze(QMap<int, QList<int> *> *liste)
{/*
    QString s = "";
    for(int i: liste) {
        s += QString::number(i)+": ";
        for (int j : liste->value(i)) {
            s += QString::number(j);
            if (j != liste->value(i)->last()) s += ", ";
        }
        s += "; ";
    }*/
    return "";
}

QMap<int, QList<int> *> *ManagerReservierungen::getPlaetzeFromString(QString plaetze)
{

}

void ManagerReservierungen::verteileSitzplaetze()
{

}

bool ManagerReservierungen::checkPlaetze(QMap<int, QList<int>*> *p)
{
    // Überprüft, ob die eingegebenen Sitzplätze frei sind, oder ob sie schon belegt wu
    return ui->checkFahrrad->isChecked();

}

void ManagerReservierungen::update()
{

}

void ManagerReservierungen::loadReservierung(Reservierung *r)
{
    nehme = false;
    aktuelleRes = r;
    ui->lineName->setText(r->getName());
    ui->lineMail->setText(r->getMail());
    ui->lineTelefon->setText(r->getTelefon());
    ui->spinAnzahl->setValue(r->getAnzahl());
    ui->comboKlasse->setCurrentIndex(r->getKlasse());
    QList<QString> *z = r->getZuege();
    QList<QString> *h = r->getHps();
    ui->comboStart1Zug->setCurrentText(z->at(0));
    ui->comboEnde1Zug->setCurrentText(z->at(1));
    ui->comboStart1Hp->setCurrentText(h->at(0));
    ui->comboEnde1Hp->setCurrentText(h->at(1));
    ui->lineSitze->setText(getStringFromPlaetze(r->getSitzplatz()));
    ui->lineSitze->setStyleSheet("background-color: #b9ceac");
    ui->checkFahrrad->setChecked(r->getFahrrad());
    ui->plainSonstiges->clear();
    ui->plainSonstiges->setPlainText(r->getSonstiges());
    nehme = true;
}

void ManagerReservierungen::saveResFahrt()
{
    QList<QString> *z = new QList<QString>();
    QList<QString> *h = new QList<QString>();
    z->append(ui->comboStart1Zug->currentText());
    z->append(ui->comboEnde1Zug->currentText());
    h->append(ui->comboStart1Hp->currentText());
    h->append(ui->comboEnde1Hp->currentText());
    aktuelleRes->setZuege(z);
    aktuelleRes->setHps(h);
}

void ManagerReservierungen::on_buttonAdd_clicked()
{
    Reservierung *r = new Reservierung();
    reservierungen->insert(r);
    QListWidgetItem *i = new QListWidgetItem(r->getName());
    ui->listRes->insertItem(0, i);
    resToItem->insert(r, i);
    itemToRes->insert(i, r);
    ui->buttonDelete->setEnabled(true);
    loadReservierung(r);
}

void ManagerReservierungen::on_buttonDelete_clicked()
{
    // Nachfrage ob wirklcih löschen
    if (QMessageBox::question(this, "Wirklich löschen?",
                              "Möchten Sie die ausgwählte Reservierung unwiderruflich löschen?"
                              ) == QMessageBox::Yes) {
        QListWidgetItem *i = ui->listRes->takeItem(ui->listRes->currentRow());
        Reservierung *r = itemToRes->value(i);
        itemToRes->remove(i);
        resToItem->remove(r);
        reservierungen->remove(r);
        if (ui->listRes->count() > 0) {
            loadReservierung(itemToRes->value(ui->listRes->item(0)));
        } else {
            nehme = false;
            ui->buttonDelete->setEnabled(false);
        }
        update();
    }
}

void ManagerReservierungen::on_listRes_itemDoubleClicked(QListWidgetItem *item)
{
    loadReservierung(itemToRes->value(item));
}

void ManagerReservierungen::on_buttonShow_clicked()
{
    QMessageBox::information(this, "Ohne Funktion", "Die Funktion ist noch nicht implementiert");
}

void ManagerReservierungen::on_buttonVerteile_clicked()
{
    QMessageBox::information(this, "Ohne Funktion", "Die Funktion ist noch nicht implementiert");
}

void ManagerReservierungen::on_checkBoxAuto_clicked(bool checked)
{
    QMessageBox::information(this, "Ohne Funktion", "Die Funktion ist noch nicht implementiert");
}

void ManagerReservierungen::on_lineName_textChanged(const QString &arg1)
{
    if (nehme) {
        aktuelleRes->setName(arg1);
        resToItem->value(aktuelleRes)->setText(aktuelleRes->getName());
    }
}

void ManagerReservierungen::on_lineMail_textChanged(const QString &arg1)
{
    if (nehme)
        aktuelleRes->setMail(arg1);
}

void ManagerReservierungen::on_lineTelefon_textChanged(const QString &arg1)
{
    if (nehme)
        aktuelleRes->setTelefon(arg1);
}

void ManagerReservierungen::on_spinAnzahl_valueChanged(int arg1)
{
    if (nehme)
        aktuelleRes->setAnzahl(arg1);
}

void ManagerReservierungen::on_comboKlasse_currentIndexChanged(int index)
{
    if (nehme)
        aktuelleRes->setKlasse(index);
}

void ManagerReservierungen::on_comboStart1Zug_currentTextChanged(const QString &arg1)
{
    if (nehme)
        saveResFahrt();
}

void ManagerReservierungen::on_comboStart1Hp_currentTextChanged(const QString &arg1)
{
    if (nehme)
        saveResFahrt();
}

void ManagerReservierungen::on_comboEnde1Zug_currentTextChanged(const QString &arg1)
{
    if (nehme)
        saveResFahrt();
}

void ManagerReservierungen::on_comboEnde1Hp_currentTextChanged(const QString &arg1)
{
    if (nehme)
        saveResFahrt();
}

void ManagerReservierungen::on_lineSitze_textChanged(const QString &arg1)
{
    if (nehme) {
        QMap<int, QList<int>*> *plaetze = getPlaetzeFromString(arg1);
        bool ok = checkPlaetze(plaetze);

        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        // Speichere die Sitzplätze, wenn die valide sind
        if (ok) {
            aktuelleRes->setSitzplatz(plaetze);
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
            update();
        } else {
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
        }
    }
}

void ManagerReservierungen::on_lineSitze_returnPressed()
{
    // prüfe, ob die sitzplätze valide sidn und speichere sie
    if (nehme) {
        QMap<int, QList<int>*> *plaetze = getPlaetzeFromString(ui->lineSitze->text());
        bool ok = checkPlaetze(plaetze);

        // Prüfe, ob die Sitzplätze valide sind und zeige dies visuell an
        // Speichere die Sitzplätze, wenn die valide sind
        if (ok) {
            aktuelleRes->setSitzplatz(plaetze);
            ui->lineSitze->setStyleSheet("background-color: #b9ceac");
            update();
        } else {
            QMessageBox::information(this, "Sitzplätze fehlerhaft", "Die eingegebenen Sitzplätze konnten nciht zugewiesen werden.\nGeben Sie gültige und freie Sitzplätze ein!");
            ui->lineSitze->setStyleSheet("background-color: #cb555d");
        }
    }
}

void ManagerReservierungen::on_checkFahrrad_clicked(bool checked)
{
    if (nehme)
        aktuelleRes->setFahrrad(checked);
}

void ManagerReservierungen::on_plainSonstiges_textChanged()
{
    if (nehme)
        aktuelleRes->setSonstiges(ui->plainSonstiges->toPlainText());
}

void ManagerReservierungen::on_listRes_itemClicked(QListWidgetItem *item)
{
    loadReservierung(itemToRes->value(ui->listRes->currentItem()));
}
