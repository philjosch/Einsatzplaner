#include "planerfahrtage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlanerFahrtage w;
    w.show();

    return a.exec();
}
