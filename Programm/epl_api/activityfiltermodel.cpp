#include "activityfiltermodel.h"

#include <QDate>

ActivityFilterModel::ActivityFilterModel()
{
    dateStart = QDateTime::currentDateTime();
    dateEnd = QDateTime::currentDateTime().addDays(30);
}

void ActivityFilterModel::setDateStart(const QDateTime &newDateStart)
{
    beginFilterChange();
    dateStart = newDateStart;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

void ActivityFilterModel::setDateEnd(const QDateTime &newDateEnd)
{
    beginFilterChange();
    dateEnd = newDateEnd;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

void ActivityFilterModel::setAcceptedTypes(const QSet<Art> &newAcceptedTypes)
{
    beginFilterChange();
    acceptedTypes = newAcceptedTypes;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

void ActivityFilterModel::setIgnoreTypes()
{
    QSet<Art> filter = QSet<Art>();
    filter << Museumszug
           << Sonderzug
           << Gesellschaftssonderzug
           << Nikolauszug
           << ELFundMuseumszug
           << Schnupperkurs
           << Bahnhofsfest
           << SonstigerFahrtag
           << Arbeitseinsatz;
    setAcceptedTypes(filter);
}

bool ActivityFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    AActivity *curr = source->getData(sourceModel()->index(sourceRow, 1, sourceParent));
    if (curr == nullptr)
        return false;
    if (curr->getBis() < dateStart)
        return false;
    if (curr->getVon() > dateEnd)
        return false;
    return (acceptedTypes.contains(curr->getArt()));
}

void ActivityFilterModel::setSource(ActivityModel *model)
{
    setSourceModel(model);
    source = model;
}

ActivityModel *ActivityFilterModel::sourceModel() const
{
    return source;
}
