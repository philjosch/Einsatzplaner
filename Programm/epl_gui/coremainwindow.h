#ifndef COREMAINWINDOW_H
#define COREMAINWINDOW_H

#include "activity.h"
#include "fahrtag.h"
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
    CoreMainWindow(QWidget *parent = nullptr);
    CoreMainWindow(EplFile *datei, QWidget *parent = nullptr);
    ~CoreMainWindow();

private:
    void constructorCoreMainWindow();

public slots:
    void autoSave();

    void loeschenPerson(Person *p);
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
    virtual void onPersonWirdEntferntWerden([[maybe_unused]] Person *p) {};
    virtual void onPersonWurdeBearbeitet([[maybe_unused]] Person *p) {};

    // Aktivitaeten bezogene Methoden
    virtual void onAktivitaetWirdEntferntWerden([[maybe_unused]] AActivity *a) {};
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
};

#endif // MAINWINDOW_H
