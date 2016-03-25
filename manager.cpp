#include "manager.h"
#include <QListWidgetItem>
#include <QMap>

Manager::Manager(QListWidget *listWidget) {
    liste = listWidget;
    fahrtage = new QList<Fahrtag*>();
}

void Manager::addFahrtag(Fahrtag *fahrtag) {
    fahrtage->append(fahrtag);
    updateFahrtag(fahrtag);
    showAll();
    liste->setCurrentItem(fahrtag->getItem());
}

void Manager::updateFahrtag(Fahrtag *fahrtag) {
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

void Manager::swap(int i, int j /* i < j*/) {
    // i < j
    liste->insertItem(j, liste->takeItem(i));
    liste->insertItem(i, liste->takeItem(j-1));
    fahrtage->swap(i, j);
}

void Manager::removeFahrtag(Fahrtag *fahrtag) {
    liste->takeItem(liste->row(fahrtag->getItem()));
    fahrtage->removeOne(fahrtag);
}

Fahrtag* Manager::getFahrtag(QListWidgetItem *item) {
    return (Fahrtag*) item->data(Qt::UserRole).value<void *>();
}
QList<Fahrtag*> *Manager::getFahrtage() {
    return fahrtage;
}
void Manager::showDate(QDate date) {
    for(int i = 0; i < fahrtage->length(); i++) {
        fahrtage->at(i)->setHidden(fahrtage->at(i)->getDatum()!=(date));
    }
}

void Manager::showAll() {
    for(int i = 0; i < fahrtage->length(); i++) {
        fahrtage->at(i)->setHidden(false);
    }
}
