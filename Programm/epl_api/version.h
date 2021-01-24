#ifndef VERSION_H
#define VERSION_H

#include <QObject>

class Version
{
public:
    Version(QString vers = "");

    Version(int maj, int min);
    Version(int maj, int min, int pat);

    //! Gibt die Version als String aus.
    QString toString();
    //! Gibt die Version im Format ``Major.Minor'' aus.
    QString toStringShort();

    int getMajor() const;
    int getMinor() const;
    int getPatch() const;

    /* Statische Methoden */
    friend bool operator==(const Version &lhs, const Version rhs);
    friend bool operator>(const Version &lhs, const Version rhs);
    friend bool operator<(const Version &lhs, const Version rhs);
    friend bool operator>=(const Version &lhs, const Version rhs);
    friend bool operator<=(const Version &lhs, const Version rhs);

    static void setVersion(Version akt, bool deploy, bool debug, QString build);
    //! Gibt die Versionsnummer des Porgramms mit allen verfuegbaren Informationen zurueck.
    static QString getKompletteVersionsnummer();
    //! Gibt die Porgrammversion zurueck;
    static Version getVersion();

    //! Testet, ob es sich um eine Entwicklerversion handelt.
    static bool isDeveloperVersion();
    //! Testet, ob die Version fuer das Debugging gebaut wurde.
    static bool isDebuggingVersion();

    static bool isSupportedVersion(Version test);


    //! Laedt die Versionsnummer des neusten Programms vom Server.
    static Version ladeNeusteVersion();
    //! Prueft, ob die angegebene Version neuer ist als die verwendete.
    static bool istAktualisierbar(Version v);

    //! Laedt die Aenderungen zur angegebenen Version vom Server.
    static QString loadNotes(Version v);

    //! Prueft, ob eine Aktualisierung vorgenommen werden kann.
    static bool isUpdateVerfuegbar();

protected:
    int major;
    int minor;
    int patch;

    static Version VERSION;
    static bool IS_DEPLOY_VERSION;
    static bool IS_DEBUG_BUILD;
    static QString BUILD_NUMBER;

    static QString URL_VERSION;
    static QString URL_NOTES;

};

#endif // VERSION_H
