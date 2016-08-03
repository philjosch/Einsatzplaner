#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include <QMainWindow>
#include "managerpersonal.h"
#include <QListWidgetItem>

namespace Ui {
class PersonalWindow;
}

class PersonalWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonalWindow(QWidget *parent, ManagerPersonal *m);
    ~PersonalWindow();

public slots:
    void showPerson(Person *p);

private slots:
    void on_pushAktualisieren_clicked();
    void refreshGesamt();
    void refreshEinzel();

    void on_pushAdd_clicked();

private:
    Ui::PersonalWindow *ui;
    ManagerPersonal *manager;

    Person *aktuellePerson;

    QHash<QListWidgetItem*, Person*> *itemToPerson;
    QHash<Person*, QListWidgetItem*> *personToItem;
};

#endif // PERSONALWINDOW_H
