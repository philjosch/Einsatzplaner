#ifndef EXPORT_H
#define EXPORT_H

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QTextDocument>
#include <QMap>

#include "aactivity.h"
#include "fahrtag.h"
#include "activity.h"
#include "person.h"
#include "managerpersonal.h"
#include "mainwindow.h"

/*
 * Die Datei mit der Logik für den Export von Daten
 * Die Daten werden über Fenster an die  methoden übergeben
 * Es werden statische Methoden benutzt, sodass die Klasse nicht instanziiert werden muss
 * */

class Export
{
public:
    // Drucken von Fahrtagen und Aktivitäten
    static bool printFahrtag(Fahrtag *f, QPrinter *pdf=0, QPrinter *paper=0);
    static bool printActivity(Activity *a, QPrinter *pdf=0, QPrinter *paper=0);

    static bool printSingle(QList<AActivity*> *liste, QPrinter *pdf=0, QPrinter *paper=0);
    static bool printList(QList<AActivity*> *liste, QPrinter *pdf=0, QPrinter *paper=0);

    // Reservierungen
    static bool printReservierung(Fahrtag *f, QPrinter *pdf=0, QPrinter *paper=0); // Gibt nur die Reservierungen aus sortiert nach Wagen und dann nach Name

    // Drucken von Personen
    static bool printPerson(Person *p, QPrinter *pdf=0, QPrinter *paper=0);
    static bool printPersonen(QList<Person *> *personen, QList<double> *gesamt, QList<bool> *data, QPrinter *pdf=0, QPrinter *paper=0);

    static QPrinter *getPrinterPaper(QWidget *parent);
    static QPrinter *getPrinterPDF(QWidget *parent, QString path);

private:
    static bool print(QPrinter *pdf, QPrinter *print, QTextDocument *d);
    static QString listToString(QMap<Person *, QString> *map, QString seperator);

    static void preparePrinterPortrait(QPrinter *p);
    static void preparePrinterLandscape(QPrinter *p);
};

#endif // EXPORT_H
