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

    // Von calendar
    QJsonObject toJson();
    QJsonObject personalToJson();
    void fromJson(QJsonObject c, QJsonObject p);

    void wheelEvent(QWheelEvent *event);


public slots:
    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);

    void open(QString path); // Open the file in a new window
    bool openFile(QString path); // Tells the window to open the file

    void unsave();
    void autoSave();


    // Von calendar
    void showDate(QDate date);

    void newFahrtag(QDate d = QDate::currentDate());
    void newActivity(QDate d = QDate::currentDate());
    bool removeActivity(AActivity *a);

    void activityChanged(AActivity *a);

    void clickedItemCalendar(AActivity *a);


signals:
    // Von calendar.h
    void showFahrtag(Fahrtag*);
    void showActivity(Activity*);
    void deleteAActivity(AActivity *);

    void changed();

protected:
    void closeEvent(QCloseEvent *event);
    bool saveToPath(QString path, bool showInMenu=true);
    // Von calendar.h
    int getItemFromDate(QDate date);
    int getPosInCalendar(QDate date);
    void insert(AActivity *a);
    void setListItemC(QListWidgetItem *i, AActivity *a);
    void setListItem(QListWidgetItem *i, AActivity *a);

    QList<CalendarDay*> tage;

    QMap<AActivity*, CalendarDay*> calendaritem; // Gibt an, in welchem Tag die Elemente momentan angezeigt werden

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

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QMap<AActivity*, QMainWindow*> fenster;

    QString filePath;
    bool saved;

    PersonalWindow *personalfenster;

    ExportGesamt *exportDialog;

    ManagerFileSettings *settings;


    Manager *manager;



};

#endif // MAINWINDOW_H
