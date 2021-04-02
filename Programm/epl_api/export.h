#ifndef EXPORT_H
#define EXPORT_H

#include "aactivity.h"
#include "fahrtag.h"
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

    // Aktivitaeten und Zubehoer
    class Aktivitaeten {
    public:
        // Drucken von Fahrtagen und Aktivitäten
        static bool printAktivitaetenEinzel(QList<AActivity*> liste, QPrinter *printer);
        static bool printAktivitaetenListe(QList<AActivity *> liste, QPrinter *printer);

        // Reservierungen
        static bool printReservierung(Fahrtag *f, QPrinter *printer); // Gibt nur die Reservierungen aus sortiert nach Wagen und dann nach Name
    };

    // Drucken von Personen mit Einsatzzeiten
    class Personal {
    public:
        static bool printZeitenEinzelEinzel(Person *p, QPrinter *printer);
        static bool printZeitenEinzelListe(QList<Person*> liste, ManagerPersonal *m, Status filter, QPrinter *printer);
        static bool printZeitenListe(QList<Person *> personen, QSet<Category> data, Status filter, QPrinter *printer);
    };

    // Mitgliederlisten
    class Mitglieder {
    public:
        static bool printMitgliederEinzelEinzel(Person *p, QPrinter *printer);
        static bool printMitgliederEinzelListe(QList<Person *> liste, ManagerPersonal *m, Status filter, QPrinter *printer);
        static bool printMitgliederListe(QList<Person *> liste, Status filter, QSet<QString> data, QPrinter *printer);
        static bool exportMitgliederAlsCSV(QList<Person *> liste, QString pfad);
    };


    // Datei-Upload
    class Upload {
    public:
        static bool uploadToServer(QList<AActivity *> liste, Networking::Server server);
        static int autoUploadToServer(QList<AActivity*> liste, Networking::Server server);
    };

    // Allgemeines
    static QPrinter *getPrinterPaper(QWidget *parent, QPrinter::Orientation orientation);
    static QPrinter *getPrinterPDF(QWidget *parent, QString path, QPrinter::Orientation orientation);

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
