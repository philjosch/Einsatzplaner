#include "guihelper.h"

QComboBox *generateNewCategoryComboBox()
{
    QComboBox *box = new QComboBox();
    box->insertItems(0, QStringList({getLocalizedStringFromCategory(Tf),
                                     getLocalizedStringFromCategory(Tb),
                                     getLocalizedStringFromCategory(Zf),
                                     getLocalizedStringFromCategory(Service),
                                     getLocalizedStringFromCategory(Zub),
                                     getLocalizedStringFromCategory(Buero),
                                     getLocalizedStringFromCategory(Werkstatt),
                                     getLocalizedStringFromCategory(ZugVorbereiten),
                                     getLocalizedStringFromCategory(Ausbildung),
                                     getLocalizedStringFromCategory(Infrastruktur),
                                     getLocalizedStringFromCategory(Sonstiges)}));
    box->setCurrentIndex(10);
    return box;
}

QTimeEdit *generateNewTimeEdit()
{
    QTimeEdit *edit = new QTimeEdit();
    edit->setDisplayFormat("hh:mm");
    return edit;
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