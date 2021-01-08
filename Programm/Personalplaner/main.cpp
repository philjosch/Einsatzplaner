#include "mainwindowpersonal.h"
#include "coreapplication.h"
#include "basics.h"

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, Version::CURRENT_API_VERSION, true, GIT_CURRENT_SHA1);

    QObject::connect(&a, &CoreApplication::triggerOpen, MainWindowPersonal::open);

    if (a.generateWindow()) {
        MainWindowPersonal *w = new MainWindowPersonal();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
