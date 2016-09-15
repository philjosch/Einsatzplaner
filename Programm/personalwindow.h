#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include <QMainWindow>
#include "managerpersonal.h"
#include <QListWidgetItem>
#include <QPrinter>

namespace Ui {
class PersonalWindow;
}

class PersonalWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonalWindow(QWidget *parent, ManagerPersonal *m);
    ~PersonalWindow();

    static const QString nichtGenugStunden;

public slots:
    void showPerson(Person *p);

private slots:
    void on_pushAktualisieren_clicked();
    void refreshGesamt();
    void refreshEinzel();

    void on_pushAdd_clicked();

    void on_lineName_textChanged(const QString &arg1);

    void on_spinKm_valueChanged(int arg1);

    void on_checkTf_clicked(bool checked);

    void on_checkZf_clicked(bool checked);

    void on_checkRangierer_clicked(bool checked);

    void on_pushDelete_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushPDF_clicked();

    void on_pushPrint_clicked();

private:
    Ui::PersonalWindow *ui;
    ManagerPersonal *manager;

    Person *aktuellePerson;

    QHash<QListWidgetItem*, Person*> *itemToPerson;
    QHash<Person*, QListWidgetItem*> *personToItem;

    void print(QPrinter *p);

    bool enabled; // Gibt an, ob das Formualr aktiviert ist oder nicht, und ob Änderungen übernommen werden
};
#endif // PERSONALWINDOW_H
