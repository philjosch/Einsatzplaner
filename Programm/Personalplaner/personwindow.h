#ifndef PERSONWINDOW_H
#define PERSONWINDOW_H

#include <QMainWindow>
#include <person.h>

namespace Ui {
class PersonWindow;
}

class PersonWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonWindow(QWidget *parent, Person *p);
    ~PersonWindow();


private slots:
    void on_lineVorname_textChanged(const QString &arg1);
    void on_lineNachname_textChanged(const QString &arg1);

    void on_lineID_textChanged(const QString &arg1);
    void on_pushAutoID_clicked();

    void on_dateGeburtstag_dateChanged(const QDate &date);
    void on_checkGeburtstag_clicked(bool checked);

    void on_dateEintritt_dateChanged(const QDate &date);
    void on_checkEintritt_clicked(bool checked);

    void on_checkAktiv_clicked(bool checked);

    void on_spinKm_valueChanged(int arg1);

    void on_lineJob_textChanged(const QString &arg1);

    void on_lineStrasse_textChanged(const QString &arg1);
    void on_linePLZ_textChanged(const QString &arg1);
    void on_lineOrt_textChanged(const QString &arg1);

    void on_linePhone_textChanged(const QString &arg1);
    void on_checkPhone_clicked(bool checked);

    void on_lineMail_textChanged(const QString &arg1);
    void on_checkMail_clicked(bool checked);

    void on_checkTf_clicked(bool checked);
    void on_checkZf_clicked(bool checked);
    void on_checkRangierer_clicked(bool checked);

    void on_dateDienst_dateChanged(const QDate &date);
    void on_checkDienst_clicked(bool checked);

    void on_plainBemerkung_textChanged();

    void on_checkAustritt_clicked(bool checked);
    void on_dateAustritt_dateChanged(const QDate &date);

    void on_pushDelete_clicked();

    void on_pushMailEinzel_clicked();

private:
    Ui::PersonWindow *ui;

    Person *person;

    bool enabled;

};

#endif // PERSONWINDOW_H
