#include "manager.h"
#include <QListWidgetItem>
#include <QMap>

Manager::Manager(QListWidget *listWidget) {
    liste = listWidget;
    mapFahrtage = new QMap<QListWidgetItem*,QFahrtag*>();
    fahrtage = new QList<QFahrtag*>();
}

void Manager::addFahrtag(QFahrtag *fahrtag, QListWidgetItem *item) {
    fahrtage->append(fahrtag);
    mapFahrtage->insert(item, fahrtag);
    updateFahrtag(fahrtag);
    showAll();
    liste->setCurrentItem(item);
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

void Manager::removeFahrtag(QFahrtag fahrtag) {
    fahrtage->removeOne(&fahrtag);
/*
    mapFahrtage->value(&fahrtag);
    QListWidgetItem * defaultValue = new QListWidgetItem();
    liste->removeItemWidget(mapFahrtage->value(&fahrtag, defaultValue));
    mapFahrtage->remove(&fahrtag);
    fahrtage->removeOne(&fahrtag);
*/
}

QFahrtag* Manager::getFahrtag(QListWidgetItem *item) {
    return mapFahrtage->value(item, new QFahrtag(new QListWidget()));
}
QList<QFahrtag*> Manager::getFahrtage() {
    return mapFahrtage->values();
}
/*
QListWidgetItem* Manager::getListItem(QFahrtag fahrtag) {
    return mapFahrtage->key(&fahrtag, new QListWidgetItem());
}
*/
void Manager::showDate(QDate date) {
    for(int i = 0; i < fahrtage->length(); i++) {
        mapFahrtage->key(fahrtage->at(i))->setHidden(fahrtage->at(i)->getDatum()!=(date));
    }
}

void Manager::showAll() {
    for(int i = 0; i < fahrtage->length(); i++) {
        mapFahrtage->key(fahrtage->at(i))->setHidden(false);
    }
}
