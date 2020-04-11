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
#include "manager.h"
#include "calendarday.h"

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

    void wheelEvent(QWheelEvent *event);

public slots:
    void newFahrtag(QDate d = QDate::currentDate());
    void newActivity(QDate d = QDate::currentDate());

    void activityChanged(AActivity *a, QDate oldDate = QDate());

    bool removeActivity(AActivity *a);

    void itemInCalendarDayClicked(AActivity *a);
    void onItemInListClicked(QListWidgetItem *item);

    void showDate(QDate date);

    void unsave();

    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);

    void open(QString path); // Open the file in a new window
    bool openFile(QString path); // Tells the window to open the file

    void autoSave();


protected:
    void closeEvent(QCloseEvent *event);
    bool saveToPath(QString path, bool showInMenu=true);
    // Von calendar.h
    void insert(AActivity *a);
    void setListItem(QListWidgetItem *i, AActivity *a);

    QList<CalendarDay*> tage;

    QMap<AActivity*, QListWidgetItem*> listitem; // Gibt an, welcher Aktivität welches Listenelement in der gesamtListe zugeordnet ist
    QMap<QListWidgetItem*, AActivity*> itemToList;

    int scrollOffSet;

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

    void on_buttonPrev_clicked();

    void on_buttonNext_clicked();

    void on_buttonToday_clicked();

    void on_buttonDelete_clicked();


private:
    Ui::MainWindow *ui;
    QMap<AActivity*, QMainWindow*> fenster;

    QString filePath;
    bool saved;

    PersonalWindow *personalfenster;

    ExportGesamt *exportDialog;

    ManagerFileSettings *settings;


    Manager *manager;

    int getPosInCalendar(QDate date);




};

#endif // MAINWINDOW_H
