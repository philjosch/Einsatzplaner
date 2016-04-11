#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"
#include <QMessageBox>
#include "fahrtag.h"


void PlanerFahrtage::createEmptyForm()
{
    // Rahmendaten
    ui->dateZug->setDate(QDate::currentDate());
    ui->comboArt->setCurrentIndex(-1);
    ui->textAnlass->clear();
    ui->checkWichtig->setChecked(false);
    ui->comboWagenreihung->setCurrentIndex(-1);
    ui->comboTimeTfH->setCurrentIndex(3);
    ui->comboTimeTfM->setCurrentIndex(0);
    ui->comboTimeZH->setCurrentIndex(3);
    ui->comboTimeZM->setCurrentIndex(0);

    // Personal
    ui->listTf->clear();
    ui->listZf->clear();
    ui->listZub->clear();
    ui->listService->clear();
    ui->textBemerkungen->clear();

    // Reservierungen
    ui->listRes->clear();
    ui->lineResName->clear();
    ui->spinResAnzahl->setValue(1);
    ui->checkResFahrrad->setChecked(false);
    ui->lineResMail->clear();
    ui->lineResTelefon->clear();
    ui->comboResKlasse->setCurrentIndex(3);
    ui->comboResStart1Zug->setCurrentIndex(1);
    ui->comboResStart1Hp->setCurrentIndex(10);
    ui->comboResEnde1Zug->setCurrentIndex(2);
    ui->comboResEnde1Hp->setCurrentIndex(10);
    ui->checkResAuto->setChecked(false);
    ui->lineResSitze->clear();
    ui->plainResSonstiges->clear();
}

void PlanerFahrtage::setState(bool aktiv)
{
    /* Rahmendaten */
    ui->dateZug->setEnabled(aktiv);
    ui->comboArt->setEnabled(aktiv);
    ui->comboWagenreihung->setEnabled(aktiv);
    ui->checkWichtig->setEnabled(aktiv);
    ui->textAnlass->setEnabled(aktiv);
    ui->textBemerkungen->setEnabled(aktiv);
    ui->comboTimeTfH->setEnabled(aktiv);
    ui->comboTimeTfM->setEnabled(aktiv);
    ui->comboTimeZH->setEnabled(aktiv);
    ui->comboTimeZM->setEnabled(aktiv);
    /* Personal */
    ui->tabPersonal->setEnabled(aktiv);
    /* Reservierungen */
    ui->tabReservierungen->setEnabled(aktiv);
    if (!aktiv) setStateRes(false);
}
void PlanerFahrtage::setStateRes(bool aktiv)
{
    ui->lineResName->setEnabled(aktiv);
    ui->lineResMail->setEnabled(aktiv);
    ui->lineResTelefon->setEnabled(aktiv);
    ui->spinResAnzahl->setEnabled(aktiv);
    ui->comboResKlasse->setEnabled(aktiv);
    ui->comboResEnde1Hp->setEnabled(aktiv);
    ui->comboResEnde1Zug->setEnabled(aktiv);
    ui->comboResStart1Hp->setEnabled(aktiv);
    ui->comboResStart1Zug->setEnabled(aktiv);
//    ui->checkResAuto->setEnabled(false);
    ui->lineResSitze->setEnabled(aktiv);
    ui->plainResSonstiges->setEnabled(aktiv);
}

QList<QString> * PlanerFahrtage::getQListFromListWidget(QListWidget *listWidget)
{
    QList<QString> *list = new QList<QString>();
    for(int i = 0; i < listWidget->count(); i++) {
        if (listWidget->item(i)->text() != tr("(Name eingeben)"))
            list->append(listWidget->item(i)->text());
    }

    return list;
}
void PlanerFahrtage::makeItemEditable(QListWidgetItem *item)
{
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
}
void PlanerFahrtage::qListToListWidget(QList<QString> *liste, QListWidget *listWidget)
{
    if (liste->size() > 0)
    {
        listWidget->clear();
        listWidget->addItems(QStringList(*liste));
        for(int i = 0; i < listWidget->count(); i++)
        {
            makeItemEditable(listWidget->item(i));
        }
    }

}

void PlanerFahrtage::deleteSelectedRowFromListWidget(QListWidget* curr)
{
    curr->takeItem(curr->currentRow());
    curr->setFocus();
}
void PlanerFahrtage::addEmptyRowToListWidget(QListWidget* curr)
{
    curr->addItem("(Name eingeben)");
    curr->item(curr->count()-1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
    curr->setCurrentItem(curr->item(curr->count()-1));
    curr->setFocus();
    curr->currentItem()->setSelected(true);
}

void PlanerFahrtage::closeEinsatzPlaner() {
    if (! saved) {
        int answ = QMessageBox::question(this, tr("Beenden?"), tr("Möchten Sie das Prgramm wirklich beenden? Die ungesicherten Information gehen verloren!"), QMessageBox::Save, QMessageBox::Close, QMessageBox::Cancel);
        if (answ == QMessageBox::Save) {
            speichern();
        } else if (answ == QMessageBox::Close) {
            saved = true;
        }
    }

    if (saved) {
        close();
    }
}
void PlanerFahrtage::closeEvent(QCloseEvent *event)
{
    closeEinsatzPlaner();
    if (saved) {
        event->accept();
    } else {
        event->ignore();
    }
}





// Neuen Fahrtag erstellen


