#ifndef MAINWINDOWEVENTS_H
#define MAINWINDOWEVENTS_H

#include "coremainwindow.h"
#include "aactivity.h"
#include "calendarday.h"
#include "exportdialog.h"
#include "personalwindow.h"

namespace Ui {
class MainWindowEvents;
}

class MainWindowEvents : public CoreMainWindow
{
    Q_OBJECT

public:
    MainWindowEvents(EplFile *file = new EplFile());
    ~MainWindowEvents();

    static bool open(QString path);

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
    void showExportDialog();
    // Bearbeiten
    void deleteSelectedInList();
    // Ansicht
    void showPersonal();

    //** Fenster
    // Kalender
    void showPreviousMonth();
    void showCurrentMonth();
    void showNextMonth();
    void showDate(QDate date);

    // Aktivitaeten
    void newFahrtag(QDate d = QDate::currentDate());
    void newActivity(QDate d = QDate::currentDate());
    void newAActivityHandler(AActivity *a);

    // Liste der Aktivitäten
    void setListItem(QListWidgetItem *i, AActivity *a);

protected:
    //** Hilfsmethoden
    int getPosInCalendar(QDate date);

    //** Modell

    //** View
    Ui::MainWindowEvents *ui;
    QList<CalendarDay*> tage;

    PersonalWindow *personalfenster;
    ExportDialog *exportDialog;

    //** Controller
    QMap<AActivity*, QListWidgetItem*> listitem; // Gibt an, welcher Aktivität welches Listenelement in der gesamtListe zugeordnet ist
    QMap<QListWidgetItem*, AActivity*> itemToList;
};

#endif // MAINWINDOWEVENTS_H
