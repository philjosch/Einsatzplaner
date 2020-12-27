#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "activity.h"
#include "fahrtag.h"
#include "calendarday.h"
#include "exportdialog.h"
#include "personalwindow.h"

#include <QCloseEvent>
#include <QJsonObject>
#include <QMainWindow>
#include <QMap>
#include "coremainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public CoreMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool handlerLadeDatei(QJsonObject json);
    ~MainWindow();

public slots:
    void newFahrtag(QDate d = QDate::currentDate());
    void newActivity(QDate d = QDate::currentDate());
    void activityChanged(AActivity *a, QDate oldDate = QDate());
    bool removeActivity(AActivity *a);

    void showDate(QDate date);

    static bool open(QString path);

protected slots:

    // Sonstige Buttons im Fenster
    void on_buttonPersonal_clicked();
    void on_buttonExport_clicked();

    // Kalender
    void on_buttonPrev_clicked();
    void on_buttonToday_clicked();
    void on_buttonNext_clicked();

    void itemInCalendarDayClicked(AActivity *a);

    // Aktivitäten
    void openAActivity(AActivity *a);
    void newAActivityHandler(AActivity *a);
    void on_buttonDelete_clicked();

    // Liste der Aktivitäten
    void setListItem(QListWidgetItem *i, AActivity *a);
    void onItemInListClicked(QListWidgetItem *item);



    CoreMainWindow *handlerNew();
    QJsonObject handlerSave();
    bool handlerSaveAdditional();
    bool handlerOpen(QString path);
    bool handlerClose();
    bool handlerPreferenes();
    bool handlerSettings();
    QJsonObject handlerSavePersonal();


private:
    bool istSchreibgeschuetzt;

    Ui::MainWindow *ui;
    QList<CalendarDay*> tage;

    QMap<AActivity*, QMainWindow*> fenster;

    PersonalWindow *personalfenster;
    ExportDialog *exportDialog;
    FileSettings *settings;

    Manager *manager;

    QString filePath;
    bool saved;

    int getPosInCalendar(QDate date);

    QMap<AActivity*, QListWidgetItem*> listitem; // Gibt an, welcher Aktivität welches Listenelement in der gesamtListe zugeordnet ist
    QMap<QListWidgetItem*, AActivity*> itemToList;
};

#endif // MAINWINDOW_H
