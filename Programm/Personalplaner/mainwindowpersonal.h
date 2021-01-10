#ifndef MAINWINDOWPERSONAL_H
#define MAINWINDOWPERSONAL_H

#include "coremainwindow.h"
#include "manager.h"
#include "personwindow.h"

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

public slots:
    static bool open(QString path);
    void refresh();


protected slots:
    CoreMainWindow *handlerNew();
    void handlerPrepareSave();
    void handlerOpen(QString path);


private slots:
    void constructor();

    void editMinimumHours();

    void on_actionAddPerson_triggered();

    void showPerson(Person *p);

    void personLoeschen(Person *p);


    void on_actionMitgliederEinzelListePDF_triggered();
    void on_actionMitgliederEinzelListeDrucken_triggered();

    void on_actionMitgliederListePDF_triggered();
    void on_actionMitgliederListeDrucken_triggered();
    void on_actionMitgliederListeCSV_triggered();

    void on_actionMailListe_triggered();

    void on_tabelleMitglieder_cellDoubleClicked(int row, int column);


private:
    Ui::MainWindowPersonal *ui;

    ManagerPersonal *personal;

    QMap<Person*, PersonWindow*> fenster;


    QList<Person*> current;
    Mitglied filter;

    QList<Person*> getSortierteListe();

    Manager *manager;

};
#endif // MAINWINDOWPERSONAL_H
