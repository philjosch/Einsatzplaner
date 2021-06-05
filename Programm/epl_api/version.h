#ifndef VERSION_H
#define VERSION_H

#include <QObject>

class Version
{
public:
    /**
     * @brief Erstellt ein neues Versions-Objekt anhand der Nummer, die in einem String kodiert wurde.
     * Liefert eine ungueltige Version, wenn das Format nicht stimmt
     * @param vers: Die Text-Darstellung der Version
     * Zulaessige Formate sind
     * - 'Major.Minor.Patch'
     * - 'Major.Minor': Die Patch Version wird auf -1 gesetzt.
     */
    Version(QString vers = "");
    Version(int maj, int min, int pat);

    /**
     * @brief Wandelt die Version in einen String um
     * @return String-Darstellung der Version
     */
    QString toString() const;
    /**
     * @brief Wandelt die Version in einen verkuerzten String um, der nicht die Patch version enthaelt
     * @return Kurze String-Darstellung der Version
     */
    QString toStringShort() const;

    int getMajor() const;
    int getMinor() const;
    int getPatch() const;

    /* Statische Methoden */
    friend bool operator==(const Version &lhs, const Version rhs);
    friend bool operator>(const Version &lhs, const Version rhs);
    friend bool operator<(const Version &lhs, const Version rhs);
    friend bool operator>=(const Version &lhs, const Version rhs);
    friend bool operator<=(const Version &lhs, const Version rhs);

    /**
     * @brief Speichert die aktuelle Programmversion
     * @param akt Die Nummerische Version
     * @param deploy true, wenn die Version offiziell freigegeben wird
     * @param debug true, wenn die Version als Debugging Version gebaut wird
     * @param build Git SHA1-Hash des letzten Commits
     */
    static void setProgrammVersion(Version akt, bool deploy, bool debug, QString build);
    /**
     * @brief getKompletteVersionsnummer
     * @return Die komplette und vollständige Versionsnummer mit allen gespeicherten Informationen
     */
    static QString getKompletteVersionsnummer();
    /**
     * @brief getVersion
     * @return Gibt den numerischen Teil der Programmversionsnummer zurueck
     */
    static Version getProgrammVersion();

    /**
     * @brief Testet, ob es sich um eine Entwicklerversion handelt.
     * @return true, wenn es eien Entwicklerversion ist.
     */
    static bool isDeveloperVersion();
    /**
     * @brief Testet, ob die Version fuer das Debugging gebaut wurde.
     * @return true, wenn fuer Debugging gebaut
     */
    static bool isDebuggingVersion();

    /**
     * @brief Prueft, ob eine Versionsnummer von dem Programm unterstuezt wird
     * @param test: Versionsnummer, fuer die geprueft wird, ob sie unterstuetzt wird
     * @return true, sofern untersuetzt
     */
    static bool isSupportedVersion(Version test);

    /**
     * @brief Prueft, ob das Porgramm aktualisiert werden kann
     * @return true, wenn eine Update verfuegbar ist.
     */
    static bool isUpdateVerfuegbar();
    /**
     * @brief Laedt die Versionsnummer des neusten Programms vom Server.
     * @return Die neuste offizielle verfuegbare Version des Programms.
     */
    static Version ladeNeusteVersion();
    /**
     * @brief Laedt die Anderungen der angegebeenn Version herunter
     * @param v: Version fuer die die Daten geladen werden sollen
     * @return QString mit den Daten
     */
    static QString loadNotes(Version v);

protected:
    /**
     * @brief Die Nummer der Hauptversion
     */
    int major;
    /**
     * @brief Die Nummer der Nebenversion
     */
    int minor;
    /**
     * @brief Die Nummer des Patches
     */
    int patch;

    /**
     * @brief Der nummerische Teil der Programmversion
     */
    static Version VERSION;
    /**
     * @brief Gibt an, ob es sich bei der Version um eine offiziell freigegebene Version handelt
     */
    static bool IS_DEPLOY_VERSION;
    /**
     * @brief Gibt an, ob die Vesion fuer das Debugging gebaut wurde.
     */
    static bool IS_DEBUG_BUILD;
    /**
     * @brief Speichert den verkürzten SHA1 Hashes des aktuellen git-Commits
     */
    static QString BUILD_NUMBER;

    /**
     * @brief Vollstaendige URL zur Datei mit der aktuellen Programmversion
     */
    static const QString URL_VERSION;
    /**
     * @brief Generische URL zu den Versionshinweisen
     * %1 dient als Platzhalter fuer die Major-Nummer der Version.
     * %2 dient als Platzhalter fuer die Minor-Nummer der Version.
     * %3 dient als Platzhalter fuer die Patch-Nummer der Version.
     */
    static const QString URL_NOTES;
};

#endif // VERSION_H
