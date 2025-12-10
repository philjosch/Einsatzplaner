#include "export.h"
#include "manager.h"

#include <QJsonArray>
#include <QListIterator>

Manager::Manager(ManagerPersonal *manPersonal) : QAbstractTableModel()
{
    activities = QList<AActivity*>();
    personal = manPersonal;
    connect(this, &Manager::changed, this, [=]() { sortData(); });
}

Manager::Manager(ManagerPersonal *manPersonal, QJsonArray array) : QAbstractTableModel()
{
    activities = QList<AActivity*>();
    personal = manPersonal;
    connect(this, &Manager::changed, this, [=]() { sortData(); });

    // Laden der Daten aus dem JSON Object
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();
        AActivity *akt;
        if (! aO.contains("isFahrtag")) {
            if (Art::Arbeitseinsatz == static_cast<Art>(aO.value("art").toInt())) {
                akt = new AActivity(aO, personal);
            } else {
                akt = new Fahrtag(aO, personal);
            }
        } else {
            // Fallback für Version < 1.6
            if (aO.value("isFahrtag").toBool()) {
                akt = new Fahrtag(aO, personal);
            } else {
                akt = new AActivity(aO, personal);
            }
        }
        newAActivityHandler(akt);
    }
}

QJsonArray Manager::toJson() const
{
    QJsonArray array;
    // Fahrtage und Arbeitseinsätze speichern
    for(AActivity *a: activities) {
        array.append(a->toJson());
    }
    return array;
}

Fahrtag *Manager::newFahrtag(QDate datum)
{
    Fahrtag *f = new Fahrtag(datum, personal);
    newAActivityHandler(f);
    return f;
}

AActivity *Manager::newActivity(QDate datum)
{
    AActivity *a = new AActivity(datum, personal);
    newAActivityHandler(a);
    return a;
}

void Manager::newAActivityHandler(AActivity *activity)
{
    beginInsertRows(QModelIndex(), activities.size(), activities.size());
    activities.append(activity);
    endInsertRows();
    sortData();
    connect(activity, &AActivity::changed, this, [=]() { emit changed(); });
    connect(activity, &AActivity::changed, this, [=](AActivity *a, [[maybe_unused]] QDate date) {
        int row = activities.indexOf(a);
        emit dataChanged(index(row, 0), index(row, columnCount()-1));
    });
    emit changed();
}

bool Manager::removeActivity(AActivity *a)
{
    int row = activities.indexOf(a);
    if (row == -1)
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    bool ret = activities.removeOne(a);
    endRemoveRows();
    delete a;
    emit changed();
    return ret;
}

QList<AActivity *> Manager::filter(Auswahl auswahl) const
{
    QList<AActivity *> liste = QList<AActivity*>();
    for (AActivity *a: activities) {
        if (a->check(auswahl))
            liste.append(a);
    }
    return liste;
}

int Manager::rowCount([[maybe_unused]] const QModelIndex &parent) const
{
    return activities.size();
}

int Manager::columnCount([[maybe_unused]] const QModelIndex &parent) const
{
    return 4;
}

QVariant Manager::data(const QModelIndex &index, int role) const
{
    if (! index.isValid())
        return QVariant();
    AActivity *current = activities.at(index.row());
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

AActivity *Manager::getData(const QModelIndex &index) const
{
    if (! index.isValid())
        return nullptr;
    return activities.at(index.row());
}

QVariant Manager::headerData(int section, Qt::Orientation orientation, int role) const
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

bool Manager::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(parent, position, position+rows-1);
    bool success = true;
    AActivity* curr;
    for (int row = 0; row < rows; ++row) {
        curr = activities.at(position);
        // removing the data shortens the list. Thus, we do not need to change the index
        success = success & removeActivity(curr);
    }
    endRemoveRows();
    return success;
}

void Manager::sortData()
{
    emit layoutAboutToBeChanged();
    AActivity::sort(&activities);
    emit layoutChanged();
}

QList<AActivity *> Manager::getActivities() const
{
    return activities;
}

bool Manager::exportActivitiesListAsHtml(QList<AActivity *> liste, ExportHtml *printer)
{
    if (printer == nullptr) return false;
    QString a = AActivity::KOPF_LISTE_HTML;
    for(AActivity *akt: liste) {
        a += akt->getHtmlForTableView();
    }
    a += AActivity::FUSS_LISTE_HTML;

    return printer->exportData(a + printer->timeStamp());
}

bool Manager::exportActivitiesDetailAsHtml(QList<AActivity *> liste, ExportHtml *printer)
{
    if (printer == nullptr) return false;
    QString html = "";
    for(AActivity *a: liste) {
        html += a->getHtmlForSingleView();
        html += printer->timeStamp(liste.last() != a);
    }
    return printer->exportData(html);
}
