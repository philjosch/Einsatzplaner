#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "coremainwindow.h"
#include "activity.h"
#include "fahrtag.h"
#include "calendarday.h"
#include "exportdialog.h"
#include "personalwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public CoreMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(EplFile *file, QWidget *parent = nullptr);
    ~MainWindow();

    static bool open(QString path);

private:
    void constructorMainWindow();

protected slots:
    //** Vererbte Methoden von CoreMainWindow
    CoreMainWindow *handlerNew();
    void handlerOpen(QString path);

    void onDateiWirdGespeichertWerden();
    void onDateiWurdeErfolgreichGespeichert();

    void onAktivitaetWirdEntferntWerden(AActivity *a);
    void onAktivitaetWurdeBearbeitet(AActivity *a, QDate altesDatum = QDate());

    //** Menueleiste
    // Datei
    void on_actionExport_triggered();
    // Bearbeiten
    void on_actionLoeschen_triggered();
    // Ansicht
    void on_actionPersonal_triggered();

    //** Fenster
    // Kalender
    void on_buttonPrev_clicked();
    void on_buttonToday_clicked();
    void on_buttonNext_clicked();
    void showDate(QDate date);

    // Aktivitaeten
    void newFahrtag(QDate d = QDate::currentDate());
    void newActivity(QDate d = QDate::currentDate());
    void openAActivity(AActivity *a);
    void newAActivityHandler(AActivity *a);

    // Liste der Aktivitäten
    void onItemInListClicked(QListWidgetItem *item);
    void setListItem(QListWidgetItem *i, AActivity *a);

protected:
    //** Hilfsmethoden
    int getPosInCalendar(QDate date);

    //** Modell

    //** View
    Ui::MainWindow *ui;
    QList<CalendarDay*> tage;
    QMap<AActivity*, QMainWindow*> fenster;

    PersonalWindow *personalfenster;
    ExportDialog *exportDialog;

    //** Controller
    QMap<AActivity*, QListWidgetItem*> listitem; // Gibt an, welcher Aktivität welches Listenelement in der gesamtListe zugeordnet ist
    QMap<QListWidgetItem*, AActivity*> itemToList;
};

#endif // MAINWINDOW_H
