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
