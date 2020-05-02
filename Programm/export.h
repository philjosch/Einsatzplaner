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
    static bool printEinzelansichten(QList<AActivity*> liste, QPrinter *printer);
    static bool printList(QList<AActivity *> liste, QPrinter *printer);

    // Reservierungen
    static bool printReservierung(Fahrtag *f, QPrinter *printer); // Gibt nur die Reservierungen aus sortiert nach Wagen und dann nach Name

    // Drucken von Personen
    static bool printPerson(ManagerPersonal *m, QPrinter *printer);
    static bool printPerson(Person *p, QPrinter *printer);
    static bool printPersonenGesamtuebersicht(QList<Person *> personen, QSet<Category> data, QPrinter *printer);

    // Mitgliederlisten
    static bool printMitglieder(ManagerPersonal *m, QPrinter *printer);

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
