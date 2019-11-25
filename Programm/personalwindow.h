#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include "managerpersonal.h"

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPrinter>

namespace Ui {
class PersonalWindow;
}

class PersonalWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonalWindow(QWidget *parent, ManagerPersonal *m);
    ~PersonalWindow();

    static const QString nichtGenugStunden;

signals:
    void changed();

public slots:
    void showPerson(Person *p);
    void refresh();

    void loadData();

private slots:
    void on_pushAktualisieren_clicked();
    void refreshGesamt();
    void refreshEinzel();

    void on_pushAdd_clicked();
    void on_pushDelete_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_lineVorname_textChanged(const QString &arg1);
    void on_lineNachname_textChanged(const QString &arg1);
    void on_spinKm_valueChanged(int arg1);
    void on_checkTf_clicked(bool checked);
    void on_checkZf_clicked(bool checked);
    void on_checkRangierer_clicked(bool checked);

    void on_pushPDF_clicked();
    void on_pushPrint_clicked();

    void on_actionSinglePrint_triggered();
    void on_actionSinglePDF_triggered();

    void on_pushPDFEinzel_clicked();
    void on_pushPrintEinzel_clicked();

    void on_tabWidgetMain_tabBarClicked(int index);

    void on_checkShowGesamt_clicked(bool checked);
    void on_checkShowAnzahl_clicked(bool checked);
    void on_checkShowTf_clicked(bool checked);
    void on_checkShowZf_clicked(bool checked);
    void on_checkShowZub_clicked(bool checked);
    void on_checkShowService_clicked(bool checked);
    void on_checkShowVorbereiten_clicked(bool checked);
    void on_checkShowWerkstatt_clicked(bool checked);
    void on_checkShowBuero_clicked(bool checked);
    void on_checkShowAusbildung_clicked(bool checked);
    void on_checkShowSonstiges_clicked(bool checked);
    void on_checkShowKilometer_clicked(bool checked);

    void on_doubleTf_valueChanged(double arg1);
    void on_doubleZf_valueChanged(double arg1);
    void on_doubleZub_valueChanged(double arg1);
    void on_doubleService_valueChanged(double arg1);
    void on_doubleZugVorbereiten_valueChanged(double arg1);
    void on_doubleWerkstatt_valueChanged(double arg1);
    void on_doubleBuero_valueChanged(double arg1);
    void on_doubleAusbildung_valueChanged(double arg1);
    void on_doubleSonstiges_valueChanged(double arg1);
    void on_doubleKilometer_valueChanged(double arg1);

    void editMinimumHours();

    void on_doubleAnzahl_valueChanged(double arg1);

private:
    Ui::PersonalWindow *ui;
    ManagerPersonal *manager;

    Person *aktuellePerson;

    QHash<QListWidgetItem*, Person*> itemToPerson;
    QHash<Person*, QListWidgetItem*> personToItem;

    QSet<Category> *anzeige;
    /* Gibt an, welche Werte in der Tabelle angezeigt werden
     * 0: gesamtstunden
     * 1: anzahl
     *  2: tf/tb
     *  3: zf
     *  4: zub/begl.o.b.a.
     *  5: service
     *  6: zug vorbereiten
     *  7: werkstatt
     *  8: büro
     *  9: sonstiges
     * 10: kilometer
     * 11: ausbildung
     * */

    void print(QPrinter *p);
    void disableFields();

    bool enabled; // Gibt an, ob das Formualr aktiviert ist oder nicht, und ob Änderungen übernommen werden

    const static QList<Category> anzeigeReihenfolge;
};
#endif // PERSONALWINDOW_H
