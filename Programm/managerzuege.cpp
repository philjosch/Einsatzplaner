#include "managerzuege.h"
#include <QListWidgetItem>
#include <QMap>
#include <QJsonObject>

ManagerZuege::ManagerZuege(QListWidget *listWidget, QListWidget *reservierungen) {
    liste = listWidget;
    resListWidget = reservierungen;
    fahrtage = new QList<Fahrtag*>();
}

Fahrtag *ManagerZuege::addFahrtag() {
    QListWidgetItem *item = new QListWidgetItem("");
    Fahrtag *fahrtag = new Fahrtag(item, resListWidget);

    liste->insertItem(0, item);
    fahrtage->insert(0, fahrtag);

    updateFahrtag(fahrtag);
    showAll();
    liste->setCurrentItem(item);

    return fahrtag;
}

void ManagerZuege::updateFahrtag(Fahrtag *fahrtag) {
    // Nach vorne bewegen
    fahrtag->update();
    int i = fahrtage->indexOf(fahrtag);
    while (i > 0 && fahrtage->at(i)->getDatum() < fahrtage->at(i-1)->getDatum()) {
        swap(i-1, i);
        i--;
    }

    // Nach hinten bewegen
    while (i < fahrtage->length()-1 && fahrtage->at(i)->getDatum() > fahrtage->at(i+1)->getDatum()) {
        swap(i, i+1);
        i++;
    }
}
void ManagerZuege::swap(int i, int j /* i < j*/) {
    // i < j
    liste->insertItem(j, liste->takeItem(i));
    liste->insertItem(i, liste->takeItem(j-1));
    fahrtage->swap(i, j);
}

void ManagerZuege::removeFahrtag(Fahrtag *fahrtag) {
    liste->takeItem(liste->row(fahrtag->getItem()));
    fahrtage->removeOne(fahrtag);
}

Fahrtag* ManagerZuege::getFahrtag(QListWidgetItem *item) {
    return (Fahrtag*) item->data(Qt::UserRole).value<void *>();
}
QList<Fahrtag*> *ManagerZuege::getFahrtage() {
    return fahrtage;
}
bool ManagerZuege::showDate(QDate date) {
    bool first = true;
    for(int i = 0; i < fahrtage->length(); i++) {
        fahrtage->at(i)->setHidden(fahrtage->at(i)->getDatum()!=(date));
        if (first && fahrtage->at(i)->getDatum()==(date)) {
            liste->setCurrentItem(fahrtage->at(i)->getItem());
            first = false;
        }
    }
    return ! first;
}

void ManagerZuege::showAll() {
    for(int i = 0; i < fahrtage->length(); i++) {
        fahrtage->at(i)->setHidden(false);
    }
}

QJsonArray ManagerZuege::toJson()
{
    QJsonArray a = QJsonArray();
    for (int i = 0; i < fahrtage->length(); i++) {
        a.append(fahrtage->at(i)->toJson());
    }
    return a;
}

void ManagerZuege::fromJson(QJsonArray a)
{
    for (int i = 0; i < a.size(); i++) {
        addFahrtag()->fromJson(a.at(i).toObject());
    }
}
