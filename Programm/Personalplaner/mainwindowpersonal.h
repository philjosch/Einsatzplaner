#ifndef MAINWINDOWPERSONAL_H
#define MAINWINDOWPERSONAL_H

#include "coremainwindow.h"
#include "manager.h"

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

private:
    void constructor();

protected slots:
    /** Vererbte Methoden von CoreMainWindow **/
    CoreMainWindow *handlerNew();
    void handlerPrepareSave();
    void handlerOpen(QString path);

    /** Menueleiste **/
    /* Bearbeiten */
    void on_actionAddPerson_triggered();
    void on_actionAktualisieren_triggered();
    void on_actionMindeststunden_triggered();
    void on_actionMailListe_triggered();

    /* Export */
    void on_actionMitgliederEinzelListePDF_triggered();
    void on_actionMitgliederEinzelListeDrucken_triggered();

    void on_actionMitgliederListePDF_triggered();
    void on_actionMitgliederListeDrucken_triggered();
    void on_actionMitgliederListeCSV_triggered();

    /** Fenster **/
    void on_comboAnzeige_currentIndexChanged(int index);

    void on_tabelleMitglieder_cellDoubleClicked(int row, int column);

protected:
    /** Hilfsmethoden **/
    void showPerson(Person *p);
    void personLoeschen(Person *p);

    QList<Person*> getSortierteListe();

    /** Modell **/
    ManagerPersonal *personal;

    /** View **/
    Ui::MainWindowPersonal *ui;
    QMap<Person*, QMainWindow*> fenster;

    /** Controller **/
    QList<Person*> current;
    Mitglied filter;

};
#endif // MAINWINDOWPERSONAL_H
