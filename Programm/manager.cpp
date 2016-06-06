#include "manager.h"

Manager::Manager()
{

}

Fahrtag *Manager::newFahrtag()
{
return new Fahrtag();
}

Activity *Manager::newActivity()
{
return new Activity();
}

bool Manager::removeActivity(Activity *a)
{

}

void Manager::addActivity(Activity *a)
{

}
