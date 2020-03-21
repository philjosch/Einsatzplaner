#ifndef MINIMUMHOURSEDITORDIALOG_H
#define MINIMUMHOURSEDITORDIALOG_H

#include "managerpersonal.h"

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class MinimumHoursEditorDialog;
}

class MinimumHoursEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MinimumHoursEditorDialog(ManagerPersonal *manager, QWidget *parent = nullptr);
    ~MinimumHoursEditorDialog();

private slots:
    void setDefault();
    void abstractButtonClickedSlot(QAbstractButton * button);
    void save();


private:
    Ui::MinimumHoursEditorDialog *ui;
    ManagerPersonal *manager;

    int timeFromString(QString arg);
    QString stringFromTime(int arg);

};

#endif // MINIMUMHOURSEDITORDIALOG_H
