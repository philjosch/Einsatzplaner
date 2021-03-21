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
     * @brief Sortiert eien Liste mit Objekten des Typs Einsatz
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
     * @brief Abfragen der tatsaechlichen Zeit, an der die Person fuer diesen Einsatz taetig wurde
     * @return Die Zeit, ab der die Person fuer diesen Einsatz tatsaechlich taetig wurde
     */
    QTime getBeginnRichtig() const;
    /**
     * @brief Abfragen der fiktiven Anfangszeit, d.h. die abweichende Zeit, wenn eine persoenliche Anfangszeit vorliegt
     * @return Die Zeit, wann diese Person fuer diesen Einsatz taetig wurde
     */
    QTime getBeginnFiktiv() const;
    /**
     * @brief Aendern der fiktiven Anfangszeit, d.h. die angepasste Zeit fuer diese Person, falls es nicht die Anfangszeit der Aktivitaet ist
     * @param value: Die zu speichernde fiktive Anfangszeit
     */
    void setBeginnFiktiv(const QTime &value);

    /**
     * @brief Abfragen der tatsaechlichen Zeit, an der die Person die Arbeit fuer diesen Einsatz beendete
     * @return Die Zeit, dis zu der die Person fuer diesen Einsatz tatsaechlich taetig war
     */
    QTime getEndeRichtig() const;
    /**
     * @brief Abfragen der fiktiven Endezeit, d.h. die abweichende Zeit, wenn eine persoenliche Endezeitzeit vorliegt
     * @return Die Zeit, wann die Person die Arbeit fuer diesen Einsatz beendete
     */
    QTime getEndeFiktiv() const;
    /**
     * @brief Aendern der fiktiven Endezeit, d.h. die angepasste Zeit fuer diese Person, falls es nicht die Endezeit der Aktivitaet ist
     * @param value: Die zu speichernde fiktive Endezeit
     */
    void setEndeFiktiv(const QTime &value);

    /**
     * @brief Abfragen, ob die Zeiten angerechnet werden sollen
     * @return true genau dann, wenn die Zeiten angerechnet werden sollen
     */
    bool getAnrechnen() const;

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
     * @brief Die gespeicherte fiktive Anfangszeit
     */
    QTime beginn;
    /**
     * @brief Die gespeicherte fiktive Endzeit
     */
    QTime ende;
};

#endif // EINSATZ_H
