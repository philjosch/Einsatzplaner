#ifndef PERSONENFILTER_H
#define PERSONENFILTER_H

#include "managerpersonal.h"
#include <QSortFilterProxyModel>

class PersonenFilter : public QSortFilterProxyModel
{
public:
    explicit PersonenFilter(QObject *parent = nullptr);

    void setModel(ManagerPersonal *model);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const override;

    Status getFilter() const;
    void setFilter(Status newFilter);

    QSet<QString> getVisibleAttributes() const;
    void setAttributeVisibility(const QString &attribute, const bool &visible);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Person *getData(QModelIndex index) const;

protected:
    QSet<QString> visibleAttributes;
    Status filter;

    ManagerPersonal *source;
};

#endif // PERSONENFILTER_H
