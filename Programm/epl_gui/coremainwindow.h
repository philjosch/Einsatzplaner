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


public slots:
    virtual void autoSave();

protected slots:
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
    void closeEvent(QCloseEvent *event);

    void unsave();

    virtual CoreMainWindow *handlerNew() = 0;
    virtual void handlerPrepareSave() {};
    virtual void handlerOnSuccessfullSave();
    virtual void handlerOpen(QString path) = 0;
    virtual bool handlerClose() {return true;};
    virtual void handlerPreferenes();
    virtual void handlerSettings() {};

    static EplFile *open(QString path);


protected:
    /** Hilfsmethoden **/
    void updateWindowHeaders();
    QList<QMainWindow*> getChildWindows();

    /** Model **/
    EplFile *datei;

    /** View **/
    QMenu *recentlyUsedMenu;
    QAction *recentlyUsedClear;
};

#endif // MAINWINDOW_H
