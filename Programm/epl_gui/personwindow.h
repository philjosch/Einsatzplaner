#ifndef PERSONWINDOW_H
#define PERSONWINDOW_H

#include "coremainwindow.h"
#include "person.h"

namespace Ui {
class PersonWindow;
}

class PersonWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonWindow(CoreMainWindow *parent, Person *p);
    ~PersonWindow();

private slots:
    // Menueleiste
    // Person
    void sendMail();

    void deleteTriggered();
    // Export
    void exportSinglePdf();
    void exportSinglePrint();


    // Felder Oberflaeche
    void changedFirstname(const QString &arg1);
    void changedFamilyname(const QString &arg1);

    void changedID(const QString &arg1);
    void autoID();

    void changedBirthday(const QDate &date);
    void changedBirthdayStatus(bool checked);

    void changedJoiningDate(const QDate &date);
    void changedJoiningStatus(bool checked);

    void changedMembershipStatus(bool checked);

    void changedDistance(int arg1);

    void changedJob(const QString &arg1);

    void changedStreet(const QString &arg1);
    void changedPostalCode(const QString &arg1);
    void changedPlace(const QString &arg1);

    void changedPhoneFirst(const QString &arg1);
    void changedPhoneStatus(bool checked);

    void changedMail(const QString &arg1);
    void changedMailStatus(bool checked);

    void changedTfStatus(bool checked);
    void changedZfStatus(bool checked);
    void changedRangiererStatus(bool checked);

    void changedFitnessDate(const QDate &date);
    void changedFitnessStatus(bool checked);

    void changedCommentsGeneral();

    void changedResignationStatus(bool checked);
    void changedResignationDate(const QDate &date);

    void changedTitle(const QString &arg1);
    void changedGender(int button);

    void changedContributionType(int index);
    void changedIBAN(const QString &arg1);
    void changedBank(const QString &arg1);
    void changedDepositor(const QString &arg1);

    void changedPhoneSecond(const QString &arg1);

    void changedCommentsOperation();
    void changedCommentsEducation();

signals:
    void loeschen(Person *);

private:
    Ui::PersonWindow *ui;

    Person *person;

    bool enabled;

};

#endif // PERSONWINDOW_H
