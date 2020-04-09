#ifndef EXPORT_H
#define EXPORT_H

#include "aactivity.h"
#include "fahrtag.h"
#include "activity.h"
#include "person.h"
#include "managerpersonal.h"
#include "mainwindow.h"

#include <QTextDocument>

/*
 * Die Datei mit der Logik für den Export von Daten
 * Die Daten werden über Fenster an die  methoden übergeben
 * Es werden statische Methoden benutzt, sodass die Klasse nicht instanziiert werden muss
 * */

class Export
{
public:
    // Drucken von Fahrtagen und Aktivitäten
    static bool print(AActivity *a, QPrinter *printer);
    static bool printSingle(QList<AActivity*> *liste, QPrinter *printer);
    static bool printList(QList<AActivity*> *liste, QPrinter *printer);

    // Reservierungen
    static bool printReservierung(Fahrtag *f, QPrinter *printer); // Gibt nur die Reservierungen aus sortiert nach Wagen und dann nach Name

    // Drucken von Personen
    static bool printPerson(ManagerPersonal *m, QPrinter *printer);
    static bool printPerson(ManagerPersonal *m, Person *p, QPrinter *printer);
    static bool printPersonen(QList<Person *> *personen, QMap<Category, int> gesamt, QList<Category> data, QPrinter *printer);

    // Allgemeines
    static QPrinter *getPrinterPaper(QWidget *parent);
    static QPrinter *getPrinterPDF(QWidget *parent, QString path);

    // Datei-Upload
    static bool testServerConnection(ManagerFileSettings *settings);
    static bool uploadToServer(ManagerFileSettings *settings, QList<AActivity*> *liste);
    static int autoUploadToServer(ManagerFileSettings *settings, Manager *mgr);

private:
    static QString listToString(QMap<Person *, QString> *map, QString seperator);

    static void preparePrinterPortrait(QPrinter *p);
    static void preparePrinterLandscape(QPrinter *p);
};

#endif // EXPORT_H
