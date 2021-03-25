#ifndef MAINWINDOWPERSONAL_H
#define MAINWINDOWPERSONAL_H

#include "coremainwindow.h"
#include "manager.h"
#include "personwindow.h"

#include <QListWidget>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowPersonal; }
QT_END_NAMESPACE

class MainWindowPersonal : public CoreMainWindow
{
    Q_OBJECT

public:
    MainWindowPersonal(QWidget *parent = nullptr);
    MainWindowPersonal(EplFile *file, QWidget *parent = nullptr);
    ~MainWindowPersonal();

    static bool open(QString path);

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

private:
    void constructorMainWindowPersonal();



protected slots:
    //** Vererbte Methoden von CoreMainWindow
    CoreMainWindow *handlerNew();
    void handlerOpen(QString path);

    void onDateiWurdeVeraendert();

    void onPersonWirdEntferntWerden(Person *p);
    void onPersonWurdeBearbeitet(Person *p);

    //** Menueleiste
    // Bearbeiten
    void on_actionAddPerson_triggered();
    void on_actionAktualisieren_triggered();
    void on_actionMindeststunden_triggered();
    void on_actionMailListe_triggered();

    // Export
    void on_actionMitgliederEinzelListePDF_triggered();
    void on_actionMitgliederEinzelListeDrucken_triggered();

    void on_actionMitgliederListePDF_triggered();
    void on_actionMitgliederListeDrucken_triggered();
    void on_actionMitgliederListeCSV_triggered();

    //** Fenster
    void on_comboAnzeige_currentIndexChanged(int index);
    void on_tabelleMitglieder_cellDoubleClicked(int row, int column);

protected:
    //** Hilfsmethoden
    void showPerson(Person *p);
    QList<Person*> getSortierteListe();

    //** Modell

    //** View
    Ui::MainWindowPersonal *ui;
    QMap<Person*, PersonWindow*> fenster;

    //** Controller
    QList<Person*> current;
    Status filter;
    QSet<QString> anzeige;

private slots:
    void on_listAnzeige_itemChanged(QListWidgetItem *item);
};
#endif // MAINWINDOWPERSONAL_H
