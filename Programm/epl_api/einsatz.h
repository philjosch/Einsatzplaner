#ifndef EINSATZ_H
#define EINSATZ_H

#include "basics.h"
#include <QObject>

class AActivity;
class Person;

class Einsatz {
public:
    /**
     * @brief Erstellt ein leeres Einsatz-Objekt
     */
    Einsatz();
    /**
     * @brief Erstellt ein leeres Einsatz-Objekt
     * @param kat: Die Kategorie welche standardmaessig vorausgewaehlt sein soll
     */
    Einsatz(Category kat);
    /**
     * @brief Erstellt ein Einsatz-Objekt mit den angegebenen Werten
     * @param p: Die Person, die verknuepft werden soll
     * @param a: Die Aktivitaet, die verknuepft werden soll
     * @param kat: Die Kategorie, auf welche die Verknuepfung gebucht wird
     * @param bem: Eine Bemerkung fuer den Einsatz
     * @param von: Optional; die individuelle Anfangszeit
     * @param bis: Optional: die individuelle Endzeit
     */
    Einsatz(Person *p, AActivity *a, Category kat, QString bem, QTime von = QTime(0,0), QTime bis = QTime(0,0));

    /**
     * @brief Sortiert eine Liste mit Objekten des Typs Einsatz
     * @param liste: Ein Pointer auf die zu sortierende Liste
     */
    static void sort(QList<Einsatz*> *liste);

    /**
     * @brief Vergleichoperator fuer kleiner gleich
     * @param lhs: Das Referenz Objekt
     * @param rhs: Das zuvergleichende Objekt
     * @return True genau dann, wenn lhs < rhs
     */
    friend bool operator<(Einsatz const & lhs, Einsatz const & rhs);


    /**
     * @brief Abfragen der Person des Einsatzes
     * @return Die abgespeicherte Person
     */
    Person *getPerson() const;
    /**
     * @brief Abfragen der Aktivitaet des Einsatzes
     * @return Die abgespeicherte Aktivitaet
     */
    AActivity *getActivity() const;
    /**
     * @brief Abfragen der Kategorie des Einsatzes
     * @return Die abgespeicherte Kategorie
     */
    Category getKategorie() const;

    /**
     * @brief Abfragen der Bemerkung des Einsatzes
     * @return Die abgespeicherte Kategorie
     */
    QString getBemerkung() const;
    /**
     * @brief Aendern der Bemerkung
     * @param value: Die neue Bemerkung des Einsatzes
     */
    void setBemerkung(const QString &value);

    /**
     * @brief Abfragen des Beginns des Einsatz
     * @return Die genaue Zeit mit Datum und Uhrzeit, wann der Einsatz begonnen hat
     */
    QDateTime getVon() const;
    /**
     * @brief Abfragen der abweichenden Anfangszeit, wenn eine persoenliche Anfangszeit vorliegt
     * @return Die von der Aktivitaet abweichende Zeit, wann diese Person fuer diesen Einsatz taetig wurde
     */
    QTime getBeginnAbweichend() const;
    /**
     * @brief Aendern der abweichenden Anfangszeit, falls es nicht die Anfangszeit der Aktivitaet ist
     * @param value: Die zu speichernde abweichende Anfangszeit
     */
    void setBeginnAbweichend(const QTime &value);

    /**
     * @brief Abfragen des Endes des Einsatz
     * @return Die genaue Zeit mit Datum und Uhrzeit, bis wann der Einsatz ging
     */
    QDateTime getBis() const;
    /**
     * @brief Abfragen der abweichenden Endezeit, wenn eine persoenliche Endezeitzeit vorliegt
     * @return Die Zeit, wann die Person die Arbeit fuer diesen Einsatz beendete
     */
    QTime getEndeAbweichend() const;
    /**
     * @brief Aendern der abweichenden Endezeit, falls es nicht die Endezeit der Aktivitaet ist
     * @param value: Die zu speichernde fiktive Endezeit
     */
    void setEndeAbweichend(const QTime &value);

    /**
     * @brief Abfragen, ob die Zeiten angerechnet werden sollen
     * @return true genau dann, wenn die Zeiten angerechnet werden sollen
     */
    bool getAnrechnen() const;

    /**
     * @brief Gibt die Dauer des Einsatzes zurück
     * @return Die Dauer des Einsatzes in Minuten.
     */
    int getDauer() const;

protected:
    static bool lesserEinsatz(const Einsatz * const &lhs, const Einsatz * const &rhs) { return (*lhs) < (*rhs); }

    /**
     * @brief Die gespeicherte Person
     */
    Person *person;
    /**
     * @brief Die gespeicherte Aktivitaet
     */
    AActivity *activity;
    /**
     * @brief Die gespeicherte Kategorie
     */
    Category kategorie;
    /**
     * @brief Die gespeicherte Bemerkung
     */
    QString bemerkung;

    /**
     * @brief Die gespeicherte abweichende Anfangszeit
     */
    QTime beginn;
    /**
     * @brief Die gespeicherte abweichende Endzeit
     */
    QTime ende;
};

#endif // EINSATZ_H
