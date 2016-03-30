#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"


void PlanerFahrtage::saveFahrplan()
{
    if (uebernehmen) {
        setSaved(false);
    }
}
