#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "activity.h"
#include "fahrtag.h"
#include "calendarday.h"
#include "exportgesamt.h"
#include "personalwindow.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(QJsonObject json, QString path);
    ~MainWindow();

public slots:
    void newFahrtag(QDate d = QDate::currentDate());
    void newActivity(QDate d = QDate::currentDate());
    void activityChanged(AActivity *a, QDate oldDate = QDate());
    bool removeActivity(AActivity *a);

    void showDate(QDate date);

    static void open(QString path);
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
    void openMostRecentlyUsedItem();
    void on_actionClear_triggered();
    void on_actionSave_triggered();
    void on_actionSaveas_triggered();
    bool saveToPath(QString path, bool showInMenu=true);
    void on_actionSavePersonal_triggered();

    void on_actionSettings_triggered();

    bool on_actionClose_triggered();
    void closeEvent(QCloseEvent *event);

    // Sonstige Buttons im Fenster
    void on_buttonPersonal_clicked();
    void on_buttonExport_clicked();

    // Kalender
    void on_buttonPrev_clicked();
    void on_buttonToday_clicked();
    void on_buttonNext_clicked();

    void itemInCalendarDayClicked(AActivity *a);

    // Aktivitäten
    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);
    void newAActivityHandler(AActivity *a);
    void on_buttonDelete_clicked();

    // Liste der Aktivitäten
    void setListItem(QListWidgetItem *i, AActivity *a);
    void onItemInListClicked(QListWidgetItem *item);

    void unsave();

private:
    Ui::MainWindow *ui;
    QList<CalendarDay*> tage;

    QMap<AActivity*, QMainWindow*> fenster;

    PersonalWindow *personalfenster;
    ExportGesamt *exportDialog;
    ManagerFileSettings *settings;

    Manager *manager;

    QString filePath;
    bool saved;

    int getPosInCalendar(QDate date);

    QMap<AActivity*, QListWidgetItem*> listitem; // Gibt an, welcher Aktivität welches Listenelement in der gesamtListe zugeordnet ist
    QMap<QListWidgetItem*, AActivity*> itemToList;
};

#endif // MAINWINDOW_H
