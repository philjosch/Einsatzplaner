#ifndef EXPORT_H
#define EXPORT_H

#include "networking.h"

#include <QTextDocument>
#include <QPrinter>

class ExportUpload;
/**
 * \class Export
 * Die Klasse mit der Logik für den Export von Daten.
 * Die Daten werden über Fenster an die Methoden übergeben.
 * Es werden statische Methoden benutzt, sodass die Klasse nicht instanziiert werden muss.
 */
class Export
{
public:
    Export(QPrinter* printer);

    /**
     * @brief getPrinterPaper erzeugt mit Hilfe eines Dialogs einen echten Drucker fuer Papierausdrucke
     * @param parent: Das Parentfenster fuer die benoetigten Dialoge
     * @param orientation: Die vorlaeufige Ausrichtung der Seite
     * @return Der fertig konfigurierte Drucker
     */
    static Export *getPrinterPaper(QWidget *parent, QPageLayout::Orientation orientation);
    /**
     * @brief getPrinterPDF erzeugt mit Hilfe eines Dialogs einen virtuellen Drucker, der in eine PDF-Datei druckt
     * @param parent: Das Parentfensters fuer die benoetigten Dialoge
     * @param fileName: Der vorgeschlagene Dateiname der PDF-Datei
     * @param orientation: Die vorlaeufige Ausrichtung der Seite
     * @return Der fertig konfigurierte PDF-Drucker
     */
    static Export *getPrinterPDF(QWidget *parent, QString fileName, QPageLayout::Orientation orientation);
    /**
     * @brief getPrinterOnline Erzeugt ein Exportobjekt das PDF-Dateien auf einem Server speichert
     * @param server Der EPL-Server, auf den die Datei hochgeladen werden soll
     * @param orientation Die Orientierung der Seiten
     * @return Das Exportobjekt
     */
    static ExportUpload *getPrinterOnline(Networking::Server server, QPageLayout::Orientation orientation);

    /**
     * @brief exportHTML druckt den angegebenen HTML-Quelltext unter zuhilfename des Standard Stylesheets
     * @param htmlString: Der Quelltext fuer den Body
     * @return WAHR genau dann, wenn der Druck erfolgreich war
     */
    virtual bool exportHTML(QString htmlString);

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

protected:
    QPrinter *printer;
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

/**
 * @brief ExportUpload ist eine Abwandlung von Export fuer den Export der Dateien auf einen Server
 */
class ExportUpload : public Export {
public:
    /**
     * @brief ExportUpload erzeugt ein Exportobjekt, welches eine mit printer erzeugte Datei auf den Server hochlaedt
     * @param printer Der Drucker zum Erzeugen der Datei
     * @param server Der Server auf den die Daten hochgeladen werden
     */
    ExportUpload(QPrinter *printer, Networking::Server server);
    bool exportHTML(QString htmlString) override;
protected:
    Networking::Server server;
};

#endif // EXPORT_H
