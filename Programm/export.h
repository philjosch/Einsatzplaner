#ifndef EXPORT_H
#define EXPORT_H

#include "aactivity.h"
#include "fahrtag.h"
#include "activity.h"
#include "person.h"
#include "managerpersonal.h"
#include "managerfilesettings.h"
#include "manager.h"

#include <QTextDocument>
#include <QPrinter>

/*
 * Die Datei mit der Logik für den Export von Daten
 * Die Daten werden über Fenster an die Methoden übergeben
 * Es werden statische Methoden benutzt, sodass die Klasse nicht instanziiert werden muss
 * */

class Export
{
public:
    // Drucken von Fahrtagen und Aktivitäten
    static bool printAktivitaetenEinzel(QList<AActivity*> liste, QPrinter *printer);
    static bool printAktivitaetenListe(QList<AActivity *> liste, QPrinter *printer);

    // Reservierungen
    static bool printReservierung(Fahrtag *f, QPrinter *printer); // Gibt nur die Reservierungen aus sortiert nach Wagen und dann nach Name

    // Drucken von Personen
    static bool printPersonenEinzelListe(QList<Person*> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer);
    static bool printPersonenEinzelEinzel(Person *p, QPrinter *printer);
    static bool printPersonenZeiten(QList<Person *> personen, QSet<Category> data, Mitglied filter, QPrinter *printer);

    // Mitgliederlisten
    static bool printMitglieder(QList<Person *> liste, Mitglied filter, QPrinter *printer);
    static bool exportMitgliederAlsCSV(ManagerPersonal *m, QList<Person *> liste, QString pfad);

    // Allgemeines
    static QPrinter *getPrinterPaper(QWidget *parent, QPrinter::Orientation orientation);
    static QPrinter *getPrinterPDF(QWidget *parent, QString path, QPrinter::Orientation orientation);

    // Datei-Upload
    static bool testServerConnection(ManagerFileSettings *settings);
    static bool uploadToServer(ManagerFileSettings *settings, QList<AActivity *> liste);
    static int autoUploadToServer(ManagerFileSettings *settings, Manager *mgr);

private:
    static void preparePrinterPortrait(QPrinter *p);
    static void preparePrinterLandscape(QPrinter *p);

    static QTextDocument *newDefaultDocument();

    static const QString DEFAULT_STYLESHEET;
    static const QFont DEFAULT_FONT;
};

#endif // EXPORT_H
