#ifndef COREMAINWINDOW_H
#define COREMAINWINDOW_H

#include "activity.h"
#include "fahrtag.h"

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
    bool ladeDatei(QString path, QJsonObject json, bool schreibschutz);
    ~CoreMainWindow();

public slots:
    void autoSave();

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
    bool saveToPath(QString path);
    void on_actionSavePersonal_triggered();

    void on_actionSettings_triggered();

    bool on_actionClose_triggered();
    void closeEvent(QCloseEvent *event);

    void unsave();

    virtual CoreMainWindow *handlerNew() = 0;
    virtual QJsonObject handlerSave() = 0;
    virtual void handlerSaveAdditional() {};
    virtual void handlerOpen(QString path) = 0;
    virtual bool handlerClose() {return true;};
    virtual void handlerPreferenes() = 0;
    virtual void handlerSettings() = 0;
    virtual QJsonObject handlerSavePersonal() = 0;
    virtual bool handlerLadeDatei(QJsonObject json) = 0;


    static bool pruefeVersionMitWarnung(Version test);
    static bool setzeSchreibschutzOderWarnung(QString pfad);


    QList<QMainWindow*> getChildWindows();

protected:
    QMenu *recentlyUsedMenu;
    QAction *recentlyUsedClear;

    bool istSchreibgeschuetzt;
    QString filePath;
    bool saved;

    void updateWindowHeaders(bool modified, QString path = "");

    void saveFailed();
};

#endif // MAINWINDOW_H
