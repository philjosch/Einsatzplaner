#ifndef ENUMS_H
#define ENUMS_H


enum Category {
    Tf,
    Tb,
    Zf,
    Service,
    Zub,
    Begleiter,
    Buero,
    Werkstatt,
    ZugVorbereiten,
    Ausbildung, // In Version 1.3 eingefuehrt
    Sonstiges=100
};

enum Mistake {
    // General errors
    OK, // Everything went right and the process was successfully done
    SonstigerFehler, // Some other misstake during the process

    // Errors regarding the management of persons
    PersonNichtGefunden, // If the person has not been found and there is no reason to interpret it as an external
    FalscheQualifikation, // registerd person was found but is not qualified to do the job
    ExternOk, // The person was not found in the system, but there is a reason to interpret it as an external

    // Errors regarding the distribution of seats
    KapazitaetUeberlauf // Thrown if there are not enough seats for the reservations
};

#endif // ENUMS_H
