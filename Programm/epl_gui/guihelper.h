#ifndef GUIHELPER_H
#define GUIHELPER_H

#include "einsatz.h"

#include <QComboBox>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QTimeEdit>

QComboBox *generateNewCategoryComboBox();
QTimeEdit *generateNewTimeEdit();

QString stringForDurationEditorFromMinutes(int m);
int minutesFromStringForDurationEditor(QString s);


class EinsatzTableWidgetItem : public QTableWidgetItem {
public:
    EinsatzTableWidgetItem(QString s) : QTableWidgetItem(s)
    {
        einsatz = new Einsatz();
    }
    EinsatzTableWidgetItem(Einsatz *e) : QTableWidgetItem() {
        einsatz = e;
    }

    Einsatz *getEinsatz() const
    {
        return einsatz;
    }
    void setEinsatz(Einsatz *value)
    {
        einsatz = value;
    }

protected:
    Einsatz *einsatz;
};

class TableListWidgetItem : public QListWidgetItem {
public:
    TableListWidgetItem(QString s) : QListWidgetItem(s)
    {
        tableItem = nullptr;
    }
    TableListWidgetItem() : QListWidgetItem()
    {
        tableItem = nullptr;
    }

    EinsatzTableWidgetItem *getTableItem() const
    {
        return tableItem;
    }
    void setTableItem(EinsatzTableWidgetItem *value)
    {
        tableItem = value;
    }

protected:
    EinsatzTableWidgetItem *tableItem;
};

class PersonTableWidgetItem : public QTableWidgetItem {
public:
    PersonTableWidgetItem(Person *p, QString s) : QTableWidgetItem(s)
    {
        person = p;
    }
    PersonTableWidgetItem(Person *p) : QTableWidgetItem() {
        person = p;
    }
    Person *getPerson() const
    {
        return person;
    }

protected: Person *person;
};

class PersonListWidgetItem : public QListWidgetItem {
public:
    PersonListWidgetItem(Person *p, QString s) : QListWidgetItem(s)
    {
        person = p;
    }
    Person *getPerson() const
    {
        return person;
    }

protected: Person *person;
};

#endif // GUIHELPER_H
