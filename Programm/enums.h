#ifndef ENUMS_H
#define ENUMS_H

#endif // ENUMS_H


enum Category { Tf,
                Tb,
                Zf,
                Service,
                Zub,
                Begleiter,
                Buero,
                Werkstatt,
                ZugVorbereiten,
                Sonstiges=100
              };
enum Mistake { OK, // Returned if the person was found in the system and is abled to do the job
                PersonNichtGefunden, // If the person has not been found and there is no reason to interpret it as an external
                FalscheQualifikation, // registerd person was found but is not qualified to do the job
                ExternOk, // The person was not found in the system, but there is a reason to interpret it as an external
                SonstigerFehler // Some other misstake during the process -> check the inputs
              };
