#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aactivity.h"
#include "fahrtag.h"
#include "fahrtagwindow.h"
#include "activity.h"
#include "activitywindow.h"
#include "personalwindow.h"
#include "exportgesamt.h"
#include "managerfilesettings.h"

#include <QMainWindow>
#include <QMap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);

    void closeAActivity(AActivity *a);

    void open(QString path); // Open the file in a new window
    bool openFile(QString path); // Tells the window to open the file

    void unsave();
    void autoSave();

protected:
    void closeEvent(QCloseEvent *event);
    bool saveToPath(QString path, bool showInMenu=true);

private slots:
    // Einsatzplaner
    void on_actionPreferences_triggered();
    void on_actionAboutQt_triggered();
    void on_actionAboutApp_triggered();
    void on_actionQuit_triggered();

    // Datei
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_menuRecentlyused_aboutToShow();
    void open();
    void on_actionClear_triggered();
    void on_actionSave_triggered();
    void on_actionSaveas_triggered();
    void on_actionSavePersonal_triggered();
    void on_actionSettings_triggered();

    bool on_actionClose_triggered();

    // Bearbeiten

    // Ansicht
    void on_buttonPersonal_clicked();
    void on_buttonExport_clicked();

private:
    Ui::MainWindow *ui;
    QMap<AActivity*, QMainWindow*> fenster;

    QString filePath;
    bool saved;

    PersonalWindow *personalfenster;

    ExportGesamt *exportDialog;

    ManagerFileSettings *settings;
};

#endif // MAINWINDOW_H
