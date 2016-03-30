#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>
#include "managerzuege.h"
#include <QTextDocument>
#include <QMap>
#include <QListWidgetItem>
#include <QPrinter>

namespace Ui {
class Export;
}

class Export : public QDialog
{
    Q_OBJECT

public:
    explicit Export(ManagerZuege *manager, QWidget *parent = 0);
    ~Export();

private slots:
    void on_comboAb_currentIndexChanged(int index);
    void on_comboBis_currentIndexChanged(int index);
    void on_ComboArt_currentIndexChanged(int index);
    void update();

    void on_buttonDrucken_clicked();
    void on_buttonPDF_clicked();

    QTextDocument *createDocSingle(QPrinter *printer);
    QTextDocument *createDocListe(QPrinter *printer);
    QString listToString(QList<QString> *liste, QString deliminiter = ", ");

    bool isAllowed(Fahrtag *f);
    bool isAllowedSingle(Fahrtag *f);

    QString stringFromReservierung(Reservierung *res);

    void on_dateBis_dateChanged(const QDate &date);

    void on_dateAb_dateChanged(const QDate &date);

private:
    Ui::Export *ui;
    ManagerZuege *manager;
    QMap<QListWidgetItem*, Fahrtag*> *map;
};

#endif // EXPORT_H
