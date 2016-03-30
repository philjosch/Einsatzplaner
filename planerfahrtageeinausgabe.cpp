#include "ui_planerfahrtage.h"
#include "planerfahrtage.h"
#include "fahrtag.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QFileDialog>
#include <QDir>
#include "export.h"

// Fahrtage exportieren
void PlanerFahrtage::on_ButtonExport_clicked()
{
    Export(zugManager, this).exec();
}

void PlanerFahrtage::setSaved(bool save)
{
    saved = save;
    setWindowModified(! save);
}

void PlanerFahrtage::oeffnen()
{
    if (! saved)
    {
        int answ = QMessageBox::question(this, tr("Datei öffnen?"), tr("Möchten Sie wirklich eine Datei öffnen? Die ungesicherten Information gehen verloren!"), QMessageBox::Save, QMessageBox::Close, QMessageBox::Cancel);
        if (answ == QMessageBox::Save)
        {
            speichern();
        } else if (answ == QMessageBox::Close)
        {
            setSaved(true);
        }
    }

    if (saved)
    {
        QString path1 = QFileDialog::getOpenFileName(this, tr("Datei öffnen..."), QDir::homePath(), tr("AkO-Dateien (*.ako)"));
        if (path1 != "")
        {
            QFile datei(path1);
            if (!datei.open(QIODevice::ReadOnly))
            {
                QMessageBox::information(this, "Fehler", "Die Datei unter dem angegebenen Pfad konnte nicht geöffnet werden!", QMessageBox::Ok);
                return;
            }
            QByteArray saveData = datei.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
            fromJson(loadDoc.object());
            ui->ListeZuege->setCurrentRow(0);
            on_ListeZuege_itemSelectionChanged();
            path = path1;
            setWindowFilePath(path);
        }
    }
}

void PlanerFahrtage::speichern()
{
    if (path == "")
    {
        speichernUnter();
    }
    else
    {
        ZugSichern();
        QFile datei(path);
        if (!datei.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, "Fehler", "Die Datei konnte nicht unter dem angegebenen Pfad gesichert werden!", QMessageBox::Ok);
        }
        else
        {
            QJsonObject o = toJson();
            QJsonDocument saveDoc = QJsonDocument(o);
            datei.write(saveDoc.toJson());
            datei.close();
            setSaved(true);
        }
    }
}
void PlanerFahrtage::speichernUnter()
{
    path = QFileDialog::getSaveFileName(this, tr("Speichern unter..."), QDir::homePath()+"/Einsatzplan.ako", tr("AkO-Dateien (*.ako)"));
    setWindowFilePath(path);
    if (path != "") speichern();
}

QJsonObject PlanerFahrtage::toJson()
{
    QJsonObject o = QJsonObject();
    o.insert("manager", zugManager->toJson());
    return o;
}
void PlanerFahrtage::fromJson(QJsonObject o)
{
    ui->ListeZuege->clear();
    zugManager = new ManagerZuege(ui->ListeZuege, ui->listRes);
    zugManager->fromJson(o.value("manager").toArray());
    for(int i = 0; i < zugManager->getFahrtage()->length(); i++)
    {
        Fahrtag *t = zugManager->getFahrtage()->at(i);
        t->getItem()->setBackgroundColor(getFarbe(t->getArt()));
        zugManager->updateFahrtag(t);
    }
    if (ui->ListeZuege->count() > 0)
    {
        ui->ButtonExport->setEnabled(true);
        ui->ButtonRemove->setEnabled(true);
    }
}
