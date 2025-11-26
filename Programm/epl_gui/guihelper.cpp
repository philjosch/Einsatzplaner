#include "guihelper.h"

QComboBox *generateNewCategoryComboBox()
{
    QComboBox *box = new QComboBox();
    box->insertItems(0, QStringList({toString(Tf),
                                     toString(Tb),
                                     toString(Zf),
                                     toString(Service),
                                     toString(Zub),
                                     toString(Buero),
                                     toString(Werkstatt),
                                     toString(ZugVorbereiten),
                                     toString(Ausbildung),
                                     toString(Infrastruktur),
                                     toString(Sonstiges)}));
    box->setCurrentIndex(10);
    return box;
}

QString stringForDurationEditorFromMinutes(int m)
{
    return QString("%1:%2").arg(int(m/60), 4, 10, QLatin1Char('0')).arg(m % 60, 2, 10,QLatin1Char('0'));
}

int minutesFromStringForDurationEditor(QString s)
{
    if (s.contains(":")) {
        QStringList l = s.split(":");
        return l.at(0).toInt()*60+l.at(1).toInt();
    }
    return s.toInt();
}
