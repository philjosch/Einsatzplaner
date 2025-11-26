#ifndef ACTIVITYWINDOW_H
#define ACTIVITYWINDOW_H

#include "aactivity.h"
#include "coremainwindow.h"

#include <QMainWindow>

namespace Ui {
class ActivityWindow;
}

class ActivityWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActivityWindow(CoreMainWindow *parent, AActivity *a);
    ~ActivityWindow();

private slots:
    void addEinsatzToTable(Einsatz *e=new Einsatz());
    void removePerson();

    void changedDate(const QDate &date);
    void changedLocation(const QString &arg1);
    void changedPurpose(const QString &arg1);
    void changedDescription();

    void changedTimeStart(const QTime &time);
    void changedTimeEnd(const QTime &time);
    void changedTimeStatus(bool checked);

    void changedImportant(bool checked);
    void changedCancelled(bool checked);

    void changedRequired(bool checked);

    void changedTableCell(int row, int column);

    void exportPrint();
    void exportPdf();

    void deleteTriggered();

signals:
    void loeschen(AActivity*);

private:
    Ui::ActivityWindow *ui;
    AActivity *activity;

    bool nehme;

    Category predefinedValueForTable;

    void setPredefinedValue(QString anlass);

    void updateWindowTitle();
};

#endif // ACTIVITYWINDOW_H
