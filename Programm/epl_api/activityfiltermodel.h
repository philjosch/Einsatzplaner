#ifndef ACTIVITYFILTERMODEL_H
#define ACTIVITYFILTERMODEL_H

#include "basics.h"
#include "manager.h"
#include <QSortFilterProxyModel>

class ActivityFilterModel : public QSortFilterProxyModel
{
public:
    ActivityFilterModel();

    void setDateStart(const QDateTime &newDateStart);
    void setDateEnd(const QDateTime &newDateEnd);
    void setAcceptedTypes(const QSet<Art> &newAcceptedTypes);
    void setIgnoreTypes();

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    void setSource(Manager *model);
    Manager *sourceModel() const;

protected:
    Manager *source;
    QDateTime dateStart;
    QDateTime dateEnd;

    QSet<Art> acceptedTypes;
};

#endif // ACTIVITYFILTERMODEL_H
