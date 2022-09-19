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
    /**
     * @brief uploadToServer laedt die Listenansicht auf den angegebenen Server
     * @param liste: Die Liste der Aktivitaeten
     * @param server: Der EPL-Server, auf den die Datei hochgeladen werden soll
     */
    static void uploadToServer(QList<AActivity *> liste, Networking::Server server);

    /**
     * @brief getPrinterPaper erzeugt mit Hilfe eines Dialogs einen echten Drucker fuer Papierausdrucke
     * @param parent: Das Parentfenster fuer die benoetigten Dialoge
     * @param orientation: Die vorlaeufige Ausrichtung der Seite
     * @return Der fertig konfigurierte Drucker
     */
    static QPrinter *getPrinterPaper(QWidget *parent, QPageLayout::Orientation orientation);
    /**
     * @brief getPrinterPDF erzeugt mit Hilfe eines Dialogs einen virtuellen Drucker, der in eine PDF-Datei druckt
     * @param parent: Das Parentfensters fuer die benoetigten Dialoge
     * @param path: Der vorgeschlagene Dateiname der PDF-Datei
     * @param orientation: Die vorlaeufige Ausrichtung der Seite
     * @return Der fertig konfigurierte PDF-Drucker
     */
    static QPrinter *getPrinterPDF(QWidget *parent, QString path, QPageLayout::Orientation orientation);

    /**
     * @brief druckeHtml druckt den angegebenen HTML-Quelltext unter zuhilfename des Standard Stylesheets
     * @param text: Der Quelltext fuer den Body
     * @param printer: Der Drucker auf dem gedruckt werden soll
     * @return WAHR genau dann, wenn der Druck erfolgreich war
     */
    static bool druckeHtml(QString text, QPrinter *printer);

    /**
     * @brief Erzeugt ein HTML-Schnipsel, das die aktuelle Zeit enthaelt
     * @param seitenUmbruch: Gibt an, ob ein Seitenumbruch danach erfolgen soll
     * @return Der HTML-Schnipsel
     */
    static QString zeitStempel(bool seitenUmbruch = false);

    /**
     * @brief preparePrinter setzt die Raender des Druckers auf die Standardwerte bzw. anhand der Ausrichtung
     * @param p: Der Drucker
     * @param orientation: DIe Ausrichtung der Seiten
     */
    static void preparePrinter(QPrinter *p, QPageLayout::Orientation orientation);

private:
    /**
     * @brief newDefaultDocument Erzeugt ein neues Textdocument mit dem standard Stylesheet
     * @return Das neue Dokument
     */
    static QTextDocument *newDefaultDocument();

    /**
     * @brief DEFAULT_STYLESHEET Speichert das standard Stylesheet
     */
    static const QString DEFAULT_STYLESHEET;
    /**
     * @brief DEFAULT_FONT Speichert die standard Schriftart, die fuer den Ausdruck verwendet wird
     */
    static const QFont DEFAULT_FONT;
};

#endif // EXPORT_H
