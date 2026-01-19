#include "personenfilter.h"

PersonenFilter::PersonenFilter(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

void PersonenFilter::setModel(ManagerPersonal *model)
{
    setSourceModel(model);
    source = model;
}

bool PersonenFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Person *currentPerson = source->getData(source->index(sourceRow, 1, sourceParent));
    if (currentPerson == nullptr)
        return false;
    return currentPerson->pruefeFilter(filter);

}
bool PersonenFilter::filterAcceptsColumn(int sourceColumn, [[maybe_unused]] const QModelIndex &sourceParent) const
{
    return visibleAttributes.contains(Person::ANZEIGE_PERSONALDATEN.at(sourceColumn));
}

Status PersonenFilter::getFilter() const
{
    return filter;
}
void PersonenFilter::setFilter(Status newFilter)
{
    beginFilterChange();
    filter = newFilter;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

QSet<QString> PersonenFilter::getVisibleAttributes() const
{
    return visibleAttributes;
}
void PersonenFilter::setAttributeVisibility(const QString &attribute, const bool &visible)
{
    beginFilterChange();
    if (visible) {
        visibleAttributes.insert(attribute);
    } else {
        visibleAttributes.remove(attribute);
    }
    endFilterChange(QSortFilterProxyModel::Direction::Columns);
}

QVariant PersonenFilter::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Vertical && role == Qt::ItemDataRole::DisplayRole) {
        return section + 1;
    }
    return QSortFilterProxyModel::headerData(section, orientation, role);
}

Person *PersonenFilter::getData(QModelIndex index) const
{
    return source->getData(mapToSource(index));
}
