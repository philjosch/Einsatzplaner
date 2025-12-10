#include "activitymodel.h"

ActivityModel::ActivityModel(Manager* manager, QObject *parent)
    : QAbstractTableModel{parent}
{
    this->manager = manager;
    connect(this->manager, &Manager::veraenderteAktivitaet, this, [=](AActivity *curr){
        int row = manager->getActivities().indexOf(curr);
        emit dataChanged(index(row, 0), index(row, columnCount()-1));
    });
    connect(this->manager, &Manager::changed, this, [=](){
        emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    });
}

int ActivityModel::rowCount(const QModelIndex &/*parent*/) const
{
    return manager->getActivities().size();
}

int ActivityModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 4;
}

QVariant ActivityModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid())
        return QVariant();
    AActivity *current = manager->getActivities().at(index.row());
    if (current == nullptr)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (index.column() == 0) {
            return current->getDatum();
        } else if (index.column() == 1) {
            return toString(current->getArt());
        } else if (index.column() == 2) {
            return current->getStringShort();
        } else if (index.column() == 3) {
            return current->getString();
        }
        break;
    case Qt::BackgroundRole:
        return QBrush(QColor(current->getFarbe()));
    case Qt::ForegroundRole:
        return QBrush(QColor("black"));
    case Qt::FontRole:
        QFont font;
        font.setBold(current->getWichtig());
        font.setStrikeOut(current->getAbgesagt());
        return font;
    }
    return QVariant();
}

AActivity* ActivityModel::getData(const QModelIndex &index) const
{
    if (! index.isValid())
        return nullptr;
    return manager->getActivities().at(index.row());
}

QVariant ActivityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "Datum";
        case 1:
            return "Art";
        case 2:
            return "Kurzbezeichnung";
        case 3:
            return "Langbezeichnung";
        }
    }
    return QVariant();
}

Fahrtag *ActivityModel::insertFahrtag(QDate date)
{

}

AActivity *ActivityModel::insertActivity(QDate date)
{

}

bool ActivityModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(parent, position, position+rows-1);
    AActivity* curr;
    for (int row = 0; row < rows; ++row) {
        curr = manager->getActivities().at(position + row);
        manager->removeActivity(curr);
    }
    endRemoveRows();
    return true;
}
