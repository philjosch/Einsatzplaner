#ifndef MANAGERZUEGE_H
#define MANAGERZUEGE_H

#include <QMap>
#include <QMapIterator>
#include <QList>

#include <QListView>
#include <QListWidgetItem>
#include <fahrtag.h>
#include <QJsonArray>


class ManagerZuege
{
public:
    ManagerZuege(QListWidget *listWidget, QListWidget *reservierungen);
public slots:
    Fahrtag *addFahrtag();
    void updateFahrtag(Fahrtag *fahrtag);
    void removeFahrtag(Fahrtag *fahrtag);
    Fahrtag* getFahrtag(QListWidgetItem *item);
    QList<Fahrtag *> *getFahrtage();
    bool showDate(QDate date);
    void showAll();

    QJsonArray toJson();
    void fromJson(QJsonArray a);


private:
    QListWidget *resListWidget;
    QListWidget *liste;
    QList<Fahrtag*> *fahrtage; // Aufsteigend sortiert nach Datum



private slots:
    void swap(int i, int j /* i < j */);



};

#endif // ManagerZuege_H
