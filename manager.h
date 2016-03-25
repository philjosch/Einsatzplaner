#ifndef MANAGER_H
#define MANAGER_H

#include <QMap>
#include <QMapIterator>
#include <QList>

#include <QListView>
#include <QListWidgetItem>
#include <fahrtag.h>


class Manager
{
public:
    Manager(QListWidget *listWidget);
public slots:
    void addFahrtag(Fahrtag *fahrtag);
    void updateFahrtag(Fahrtag *fahrtag);
    void removeFahrtag(Fahrtag *fahrtag);
    Fahrtag* getFahrtag(QListWidgetItem *item);
    QList<Fahrtag *> *getFahrtage();
    void showDate(QDate date);
    void showAll();

private:
    QListWidget *liste;
    QList<Fahrtag*> *fahrtage; // Aufsteigend sortiert nach Datum



private slots:
    void swap(int i, int j /* i < j */);



};

#endif // MANAGER_H
