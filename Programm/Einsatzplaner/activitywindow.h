#ifndef ACTIVITYWINDOW_H
#define ACTIVITYWINDOW_H

#include "aactivity.h"
#include "coremainwindow.h"

#include <QMainWindow>
#include <QTableWidgetItem>

namespace Ui {
class ActivityWindow;
}

class ActivityWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActivityWindow(CoreMainWindow *parent, AActivity *a);
    ~ActivityWindow();

    class EinsatzTableWidgetItem : public QTableWidgetItem {
    public:
        EinsatzTableWidgetItem(QString s) : QTableWidgetItem(s)
        {
            einsatz = AActivity::Einsatz();
        }
        EinsatzTableWidgetItem(AActivity::Einsatz e) : QTableWidgetItem() {
            einsatz = e;
        }

        AActivity::Einsatz getEinsatz() const
        {
            return einsatz;
        }
        void setEinsatz(AActivity::Einsatz value)
        {
            einsatz = value;
        }

    protected:
        AActivity::Einsatz einsatz;
    };


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

    void on_checkWichtig_clicked(bool checked);
    void on_checkAbgesagt_clicked(bool checked);

    void on_checkBoxBenoetigt_toggled(bool checked);

    void on_tablePersonen_cellChanged(int row, int column);

    void on_actionPrint_triggered();
    void on_actionPdf_triggered();

    void on_actionDelete_triggered();

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
