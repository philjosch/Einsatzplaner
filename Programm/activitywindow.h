#ifndef ACTIVITYWINDOW_H
#define ACTIVITYWINDOW_H

#include "activity.h"
#include "person.h"

#include <QMainWindow>

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
    void on_lineAnlass_textChanged(const QString &arg1);
    void on_plainBeschreibung_textChanged();

    void on_timeBeginn_timeChanged(const QTime &time);
    void on_timeEnde_timeChanged(const QTime &time);
    void on_checkZeiten_clicked(bool checked);

    void on_checkBoxBenoetigt_toggled(bool checked);

    void on_tablePersonen_cellChanged(int row, int column);
    void comboInTableChanged();
    void timeEditInTableChanged();

    void on_actionDelete_triggered();
    void on_actionPrint_triggered();
    void on_actionPdf_triggered();

    void on_buttonDelete_clicked();


private:
    Ui::ActivityWindow *ui;
    Activity *activity;

    bool nehme;

    void loadData();

    QSet<QString> namen;
    Category predefinedValueForTable;

    void setPredefinedValue(QString anlass);
};

#endif // ACTIVITYWINDOW_H
