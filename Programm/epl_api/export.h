#ifndef EXPORT_H
#define EXPORT_H

#include "networking.h"

#include <QTextDocument>
#include <QPrinter>
#include <QTemporaryFile>
#include <QPdfWriter>

/**
 * @brief The Export class provides an abstract interface to export data to some type of output device
 * The actual implementations of the device depend on the subclass implementing the interface
 */
class Export {
public:
    enum ContentFormat {
        Undefined
        , HTML
        , CSV
        , JSON
    };

    /**
     * @brief exportData exports the given data to the predefined location
     * @param data The data to be exported, the content format is not checked and treated as valid
     * @return true if the export succeeded
     */
    virtual bool exportData(QString data) = 0;

    /**
     * @brief getRequiredFormat Returns the format of the content the device is able to process
     * @return The format of the content the device is processing
     */
    ContentFormat getRequiredFormat() { return format; };

    /**
     * @brief prepareLayout adjusts the margings of the painting device to default values according to the orientation
     * @param device The device to be adjusted
     * @param orientation The default orientation of the pages
     */
    static void prepareLayout(QPagedPaintDevice *device, QPageLayout::Orientation orientation);

    static QPrinter *generatePrinter(QWidget *parent, QPageLayout::Orientation orientation);

protected:
    ContentFormat format;
};


/**
 * @brief The ExportHtml class provides an interface to export HTML content to some device
 */
class ExportHtml : public Export
{
public:
    ExportHtml();
    /**
     * @brief Creates an HTML snippet containing the current time
     * @param pageBreak: true if a page break should appear afterward
     * @return The HTML snippet
     */
    static QString timeStamp(bool pageBreak = false);

protected:
    /**
     * @brief newDefaultDocument Creates a QTextDocument with the default template
     * @return The new document
     */
    static QTextDocument *newDefaultDocument();

    /**
     * @brief DEFAULT_STYLESHEET Stores the default CSS template for HTML export
     */
    static const QString DEFAULT_STYLESHEET;
    /**
     * @brief DEFAULT_FONT Stores the default font for HTML export
     */
    static const QFont DEFAULT_FONT;
};

/**
 * @brief The ExportHtmlPaper class provides the option to print HTML content on a printer
 */
class ExportHtmlPaper: public ExportHtml{
public:
    ExportHtmlPaper(QPrinter *printer);

    /**
     * @brief generate creates an export device via a printer dialog for paper output
     * @param parent: The parent window which should be used for the dialoge
     * @param orientation: The default orientation of the pages
     * @return The final printer as an export device
     */
    static ExportHtmlPaper *generate(QWidget *parent, QPageLayout::Orientation orientation);

    bool exportData(QString data) override;

protected:
    QPrinter *printer;
};

/**
 * @brief The ExportHtmlPdf class provides the option to export a PDF based on HTML content
 */
class ExportHtmlPdf: public ExportHtml {
public:
    ExportHtmlPdf(QString filePath, QPageLayout::Orientation orientation);
    bool exportData(QString data) override;

    /**
     * @brief generate creates an HTML export device that stores the content in a PDF file
     * @param parent: The parent window which should be used for the dialog
     * @param fileName: The default filename for the PDF, not a path
     * @param orientation: The default orientation of the pages
     * @return Der final export device
     */
    static ExportHtmlPdf *generate(QWidget *parent, QString fileName, QPageLayout::Orientation orientation);

protected:
    QPdfWriter *printer;
};


/**
 * @brief ExportHtmlUpload is a modified HTML export device which stores the file on a EPL-server
 */
class ExportHtmlUpload : public ExportHtml {
public:
    /**
     * @brief generate creates an HTML export device which stores the content as a PDF on the configures server
     * @param server The configuration of the EPL server to which the file should be uploaded
     * @param orientation The default orientation of the pages
     * @return The final export device
     */
    [[deprecated("Directly use constructor as no GUI-interaction is needed.")]]
    static ExportHtmlUpload *generate(Networking::Server server, QPageLayout::Orientation orientation);

    /**
     * @brief ExpportHtmlUpload initializes an HTML export device which stores the content as a PDF on the configures server
     * @param server The configuration of the EPL server to which the file should be uploaded
     * @param orientation The default orientation of the pages
     */
    ExportHtmlUpload(Networking::Server server, QPageLayout::Orientation orientation);
    bool exportData(QString data) override;


protected:
    ExportHtmlPdf *printer;
    QTemporaryFile *tempFile;
    Networking::Server server;
};

/**
 * @brief The ExportCsv class provides an export of CSV values to some export device
 */
class ExportCsv: public Export {
public:
    ExportCsv();
};

/**
 * @brief The ExportCsvFile class is an export device which stores CSV data in a CSV file
 */
class ExportCsvFile: public ExportCsv {
public:
    /**
     * @brief ExportCsvFile creates an export device which stores the data in a specified file
     * @param path The full path for the file in which the data should be stores
     */
    ExportCsvFile (QString path);
    /**
     * @brief generate creates an CSV export device
     * @param parent The parent window which should be used for the dialog
     * @param fileName The default file name for the CSV file, not a path
     * @return The final export devide
     */
    static ExportCsvFile *generate(QWidget *parent, QString fileName);

    bool exportData(QString data) override;

protected:
    QString filePath;
};

#endif // EXPORT_H
