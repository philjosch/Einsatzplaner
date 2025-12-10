#ifndef ACTIVITYMODEL_H
#define ACTIVITYMODEL_H

#include "manager.h"
#include <QAbstractListModel>

class ActivityModel : public QAbstractTableModel
{
public:
    explicit ActivityModel(Manager *manager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    AActivity *getData(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Fahrtag *insertFahrtag(QDate date);
    AActivity *insertActivity(QDate date);
    bool removeRows(int position, int rows, const QModelIndex &parent) override;

protected:
    Manager *manager;
};

#endif // ACTIVITYMODEL_H
