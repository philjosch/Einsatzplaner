#ifndef EXPORT_H
#define EXPORT_H

#include "aactivity.h"
#include "fahrtag.h"
#include "activity.h"
#include "person.h"
#include "managerpersonal.h"
#include "filesettings.h"
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
    static bool printZeitenEinzelEinzel(Person *p, QPrinter *printer);
    static bool printZeitenEinzelListe(QList<Person*> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer);
    static bool printZeitenListe(QList<Person *> personen, QSet<Category> data, Mitglied filter, QPrinter *printer);

    // Mitgliederlisten
    static bool printMitgliederEinzelEinzel(Person *p, QPrinter *printer);
    static bool printMitgliederEinzelListe(QList<Person *> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer);
    static bool printMitgliederListe(QList<Person *> liste, Mitglied filter, QPrinter *printer);
    static bool exportMitgliederAlsCSV(QList<Person *> liste, QString pfad);

    // Allgemeines
    static QPrinter *getPrinterPaper(QWidget *parent, QPrinter::Orientation orientation);
    static QPrinter *getPrinterPDF(QWidget *parent, QString path, QPrinter::Orientation orientation);

    // Datei-Upload
    static bool uploadToServer(QList<AActivity *> liste, Networking::Server server);
    static int autoUploadToServer(QList<AActivity*> liste, Networking::Server server);

    // Helfer-Methoden, die auch Extern verwendet werden koennen
    static bool druckeHtmlAufDrucker(QString text, QPrinter *printer);

private:
    static void preparePrinter(QPrinter *p, QPrinter::Orientation orientation);

    static QString zeitStempel();

    static QTextDocument *newDefaultDocument();

    static const QString DEFAULT_STYLESHEET;
    static const QFont DEFAULT_FONT;
};

#endif // EXPORT_H