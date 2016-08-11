#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QFrame>
#include <QListWidgetItem>
#include "reservierung.h"

namespace Ui {
class ManagerReservierungen;
}

class ManagerReservierungen: public QFrame
{
    Q_OBJECT

public:
    explicit ManagerReservierungen(QWidget *parent = 0);
    ~ManagerReservierungen();

    QString getWagenreihung() const;
    void setWagenreihung(const QString &value);

    static QString getStringFromPlaetze(QMap<int, QList<int> *> *liste);
    static QMap<int, QList<int>*> *getPlaetzeFromString(QString plaetze);

public slots:
    void verteileSitzplaetze();
    bool checkPlaetze(QMap<int, QList<int> *> *p);

    void update(); // Wird aufgerufen, wenn sich etwas an den Sitzplaetzen veraendert hat: mehr, wengier, andere, ...

private:
    Ui::ManagerReservierungen *ui;
    Reservierung *aktuelleRes;
    bool nehme;

    void loadReservierung(Reservierung *r);
    void saveResFahrt();

protected:
    QString wagenreihung;
    QSet<Reservierung*> *reservierungen;
    QMap<Reservierung*, QListWidgetItem*> *resToItem;
    QMap<QListWidgetItem*, Reservierung*> *itemToRes;


private slots:
    void on_buttonAdd_clicked();
    void on_buttonDelete_clicked();
    void on_listRes_itemDoubleClicked(QListWidgetItem *item);
    void on_buttonShow_clicked();
    void on_buttonVerteile_clicked();
    void on_checkBoxAuto_clicked(bool checked);
    void on_lineName_textChanged(const QString &arg1);
    void on_lineMail_textChanged(const QString &arg1);
    void on_lineTelefon_textChanged(const QString &arg1);
    void on_spinAnzahl_valueChanged(int arg1);
    void on_comboKlasse_currentIndexChanged(int index);
    void on_comboStart1Zug_currentTextChanged(const QString &arg1);
    void on_comboStart1Hp_currentTextChanged(const QString &arg1);
    void on_comboEnde1Zug_currentTextChanged(const QString &arg1);
    void on_comboEnde1Hp_currentTextChanged(const QString &arg1);
    void on_lineSitze_textChanged(const QString &arg1);
    void on_lineSitze_returnPressed();
    void on_checkFahrrad_clicked(bool checked);
    void on_plainSonstiges_textChanged();
    void on_listRes_itemClicked(QListWidgetItem *item);
};

#endif // MANAGERRESERVIERUNGEN_H
