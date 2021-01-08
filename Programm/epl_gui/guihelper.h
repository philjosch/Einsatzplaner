#ifndef GUIHELPER_H
#define GUIHELPER_H

#include "aactivity.h"

#include <QComboBox>
#include <QTimeEdit>

QComboBox *generateNewCategoryComboBox();
QTimeEdit *generateNewTimeEdit();

QString stringForDurationEditorFromMinutes(int m);
int minutesFromStringForDurationEditor(QString s);


#endif // GUIHELPER_H
