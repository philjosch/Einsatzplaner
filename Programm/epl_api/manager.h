#ifndef MANAGER_H
#define MANAGER_H

#include "fahrtag.h"
#include "managerpersonal.h"

#include <QJsonObject>
#include <QAbstractTableModel>

class Manager : public QAbstractTableModel
{
    Q_OBJECT

public:
    Manager(ManagerPersonal *manPersonal);
    Manager(ManagerPersonal *manPersonal, QJsonArray array);
    QJsonArray toJson() const;

    QList<AActivity *> getActivities() const;

    static bool exportActivitiesListAsHtml(QList<AActivity *> liste, ExportHtml *printer);
    static bool exportActivitiesDetailAsHtml(QList<AActivity *> liste, ExportHtml *printer);

    Fahrtag *newFahrtag(QDate datum);
    AActivity *newActivity(QDate datum);

    bool removeActivity(AActivity *a);

    QList<AActivity*> filter(Auswahl auswahl) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    AActivity *getData(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;

signals:
    void changed();
    void veraenderteAktivitaet(AActivity*, QDate);

protected:
    QList<AActivity *> activities;
    ManagerPersonal *personal;

    void newAActivityHandler(AActivity *activity);
    void sortData();
};

#endif // MANAGER_H
