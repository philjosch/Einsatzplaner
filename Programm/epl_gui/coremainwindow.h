#ifndef COREMAINWINDOW_H
#define COREMAINWINDOW_H

#include "aactivity.h"
#include "eplexception.h"
#include "eplfile.h"

#include <QAction>
#include <QCloseEvent>
#include <QJsonObject>
#include <QMainWindow>
#include <QMap>
#include <QMenu>

class CoreMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CoreMainWindow(EplFile *datei = new EplFile());
    ~CoreMainWindow();

public slots:
    void autoSave();

    bool loeschenPerson(Person *p);
    void loeschenAktivitaet(AActivity *a);

protected slots:
    //** Datei Menue
    // Einsatzplaner
    void on_actionPreferences_triggered();
    void on_actionAboutQt_triggered();
    void on_actionAboutApp_triggered();
    void on_actionQuit_triggered();

    // Datei
    void on_actionNew_triggered();
    void on_actionOpen_triggered();

    void on_menuRecentlyused_aboutToShow();
    void on_actionClear_triggered();

    void on_actionSave_triggered();
    void on_actionSaveas_triggered();
    void on_actionSavePersonal_triggered();

    void on_actionSettings_triggered();
    bool on_actionClose_triggered();


    //** Dialog-Handling
    virtual void handlerPreferenes();
    virtual void handlerSettings();

    //** Virtuelle Methoden
    // Fenster schliessen
    virtual bool onFensterSollGeschlossenWerden() {return true;};

    // Dateien erstellen oder laden
    virtual CoreMainWindow *handlerNew() = 0;
    virtual void handlerOpen(QString path) = 0;

    // Dateien bezogene Methoden
    virtual void onDateiWurdeVeraendert();
    virtual void onDateiWirdGespeichertWerden() {};
    virtual void onDateiWurdeErfolgreichGespeichert();

    // Personen bezogene Methoden
    virtual void openPerson(Person *p);
    virtual void onPersonWirdEntferntWerden(Person *p);
    virtual void onPersonWurdeBearbeitet([[maybe_unused]] Person *p) {};

    // Aktivitaeten bezogene Methoden
    virtual void openAktivitaet(AActivity *a);
    virtual void onAktivitaetWirdEntferntWerden([[maybe_unused]] AActivity *a);
    virtual void onAktivitaetWurdeBearbeitet([[maybe_unused]] AActivity *a, [[maybe_unused]] QDate altesDatum = QDate()) {};

protected:
    //** Hilfsmethoden
    void closeEvent(QCloseEvent *event);

    static EplFile *getDateiVonPfad(QString path);
    QList<QMainWindow*> getChildWindows();
    void updateWindowHeaders();

    //** Modell
    EplFile *datei;
    Manager *manager;
    ManagerPersonal *personal;

    //** View
    QMenu *recentlyUsedMenu;
    QAction *recentlyUsedClear;

    //** Controller
    QMap<AActivity*, QMainWindow*> fensterActivities;
    QMap<Person*, QMainWindow*> fensterPersonen;
};

#endif // MAINWINDOW_H
