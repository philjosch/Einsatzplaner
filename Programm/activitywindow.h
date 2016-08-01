#ifndef ACTIVITYWINDOW_H
#define ACTIVITYWINDOW_H

#include <QMainWindow>
#include "activity.h"

namespace Ui {
class ActivityWindow;
}

class ActivityWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActivityWindow(QWidget *parent, Activity *a);
    ~ActivityWindow();

private slots:
    void on_buttonInsert_clicked();

    void on_buttonRemove_clicked();

    void on_dateDatum_dateChanged(const QDate &date);

    void on_lineOrt_textChanged(const QString &arg1);

    void on_plainTextEdit_textChanged();

    void on_plainBeschreibung_textChanged();

    void on_timeBeginn_timeChanged(const QTime &time);

    void on_timeEnde_timeChanged(const QTime &time);

    void on_checkBoxBenoetigt_toggled(bool checked);

    void on_tablePersonen_cellChanged(int row, int column);

private:
    Ui::ActivityWindow *ui;
    Activity *activity;
};

#endif // ACTIVITYWINDOW_H
