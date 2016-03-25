#ifndef MANAGER_H
#define MANAGER_H

#include <QMap>
#include <QMapIterator>
#include <QList>

#include <QListWidget>
#include <QListWidgetItem>
#include <qfahrtag.h>


class Manager
{
public:
    Manager(QListWidget *listWidget);
public slots:
    void addFahrtag(QFahrtag *fahrtag, QListWidgetItem *item);
    void updateFahrtag(QFahrtag *fahrtag);
    void removeFahrtag(QFahrtag fahrtag);
    QFahrtag* getFahrtag(QListWidgetItem *item);
    QList<QFahrtag*> getFahrtage();
//    QListWidgetItem *getListItem(QFahrtag fahrtag);
    void showDate(QDate date);
    void showAll();

private:
    QListWidget *liste;
    QMap<QListWidgetItem*, QFahrtag*> *mapFahrtage;
    QList<QFahrtag*> *fahrtage; // Aufsteigend sortiert nach Datum



private slots:
    void swap(int i, int j /* i < j */);



};

#endif // MANAGER_H
