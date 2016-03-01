#include "manager.h"

Manager::Manager(QListWidget *listWidget) {
    liste = listWidget;
    mapFahrtage = new QMap<QFahrtag*,QListWidgetItem*>();
    fahrtage = new QList<QFahrtag*>();
}

void Manager::addFahrtag(QFahrtag *fahrtag, QListWidgetItem *item) {
    fahrtage->append(fahrtag);
    mapFahrtage->insert(fahrtag, item);
    updateFahrtag(fahrtag);
}

void Manager::updateFahrtag(QFahrtag *fahrtag) {
    // Nach vorne bewegen
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

void Manager::swap(int i, int j /* i < j*/) {
    // i < j
    liste->insertItem(j, liste->takeItem(i));
    liste->insertItem(i, liste->takeItem(j-1));
    fahrtage->swap(i, j);
}

void Manager::removeFahrtag(QFahrtag *fahrtag) {
    liste->removeItemWidget(mapFahrtage->value(fahrtag));
    mapFahrtage->remove(fahrtag);
    fahrtage->removeOne(fahrtag);
}

QFahrtag* Manager::getFahrtag(QListWidgetItem *item) {
    return mapFahrtage->key(item);
}
QList<QFahrtag*> Manager::getFahrtage() {
    return mapFahrtage->keys();
}

QListWidgetItem* Manager::getListItem(QFahrtag *fahrtag) {
    return mapFahrtage->value(fahrtag);
}
