#ifndef EXPORT_H
#define EXPORT_H

#include "aactivity.h"
#include "filesettings.h"

#include <QTextDocument>
#include <QPrinter>

/**
 * \class Export
 * Die Klasse mit der Logik für den Export von Daten.
 * Die Daten werden über Fenster an die Methoden übergeben.
 * Es werden statische Methoden benutzt, sodass die Klasse nicht instanziiert werden muss.
 */
class Export
{
public:
    // Datei-Upload
    class Upload {
    public:
        static void uploadToServer(QList<AActivity *> liste, Networking::Server server);
        static void autoUploadToServer(QList<AActivity*> liste, Networking::Server server);
    };

    // Allgemeines

    static QPrinter *getPrinterPaper(QWidget *parent, QPageLayout::Orientation orientation);
    static QPrinter *getPrinterPDF(QWidget *parent, QString path, QPageLayout::Orientation orientation);

    // Helfer-Methoden, die auch Extern verwendet werden koennen
    static bool druckeHtmlAufDrucker(QString text, QPrinter *printer);

    /**
     * @brief Erzeugt ein HTML-Schnipsel, das die aktuelle Zeit enthaelt
     * @param seitenUmbruch: Gibt an, ob ein Seitenumbruch danach erfolgen soll
     * @return Der HTML-Schnipsel
     */
    static QString zeitStempel(bool seitenUmbruch = false);

private:
    static void preparePrinter(QPrinter *p, QPageLayout::Orientation orientation);

    static QTextDocument *newDefaultDocument();

    static const QString DEFAULT_STYLESHEET;
    static const QFont DEFAULT_FONT;
};

#endif // EXPORT_H
