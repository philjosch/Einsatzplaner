#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "aactivity.h"
#include "fahrtag.h"
#include "fahrtagwindow.h"
#include "activity.h"
#include "activitywindow.h"
#include "personalwindow.h"
#include "exportgesamt.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static QString getFarbeZug(Fahrtag::Art cat);
    static QString getFarbeArbeit();
    static QString getFarbe(AActivity *a);

private:
    Ui::MainWindow *ui;
    QMap<AActivity*, QMainWindow*> *fenster;

    QString filePath;
    bool saved;

    PersonalWindow *personalfenster;

    ExportGesamt *exportDialog;

public slots:
    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);

    void open(QString filepath); // Open the file in a new window
    bool openFile(QString filePath); // Tells the window to open the file

    void unsave();

protected:
    void closeEvent(QCloseEvent *event);

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
    bool on_actionClose_triggered();

    // Bearbeiten

    // Ansicht
    void on_buttonPersonal_clicked();
    void on_buttonExport_clicked();
};

#endif // MAINWINDOW_H
