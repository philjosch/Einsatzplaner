#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include "managerpersonal.h"

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPrinter>
#include <QTableWidgetItem>
#include <coremainwindow.h>

namespace Ui {
class PersonalWindow;
}

class PersonalWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonalWindow(CoreMainWindow *parent, ManagerPersonal *m);
    ~PersonalWindow();

public slots:
    void refresh();

private slots:
    void refreshTabelle();
    void refreshEinzel();

    // Personalmanager
    void addPerson();
    void editMinimumhours();

    // Export
    void exportTimesDetailOnePdf();
    void exportTimesDetailOnePrint();

    void exportTimesDetailMultiplePdf();
    void exportTimesDetailMultiplePrint();

    void exportTimesListPdf();
    void exportTimesListPrint();


    void exportMemberDetailOnePdf();
    void exportMemberDetailOnePrint();

    void exportMemberDetailMultiplePdf();
    void exportMemberDetailMultiplePrint();

    void exportMemberListPdf();
    void exportMemberListPrint();
    void exportMemberListCsv();


    void exportDuesRegularCsv();
    void exportDuesAdditionalCsv();

    // Fenster - Gesamt
    void sendMailList();

    void persShowFromTable(int row, int column);

    void viewShowGesamt(bool checked);
    void viewShowAnzahl(bool checked);
    void viewShowTf(bool checked);
    void viewShowTb(bool checked);
    void viewShowZf(bool checked);
    void viewShowZub(bool checked);
    void viewShowService(bool checked);
    void viewShowVorbereiten(bool checked);
    void viewShowWerkstatt(bool checked);
    void viewShowBuero(bool checked);
    void viewShowAusbildung(bool checked);
    void viewShowInfrastruktur(bool checked);
    void viewShowSonstiges(bool checked);
    void viewShowKilometer(bool checked);


    // Fenster - Einzel

    void persShowFromList(QListWidgetItem *item);

    void persSetVorname(const QString &arg1);
    void persSetNachname(const QString &arg1);

    void persSetAktiv(bool checked);

    void persSetTf(bool checked);
    void persSetZf(bool checked);
    void persSetRangierer(bool checked);

    void persSetDienst(const QDate &date);
    void persSetDienstUnkown(bool checked);

    void persSetBemerkung();
    void persSetAusbildung();
    void persSetBetrieb();

    void persDelete();

    void persShowDetails();

    void persSetAdditionalTf(const QString &arg1);
    void persSetAdditionalTb(const QString &arg1);
    void persSetAdditionalZf(const QString &arg1);
    void persSetAdditionalZub(const QString &arg1);
    void persSetAdditionalService(const QString &arg1);
    void persSetAdditionalVorbereiten(const QString &arg1);
    void persSetAdditionalWerkstatt(const QString &arg1);
    void persSetAdditionalBuero(const QString &arg1);
    void persSetAdditionalAusbildung(const QString &arg1);
    void persSetAdditionalInfrastruktur(const QString &arg1);
    void persSetAdditionalSonstiges(const QString &arg1);
    void persSetAdditionalAnzahl(double arg1);
    void persSetAdditionalKilometer(double arg1);


    void showPerson(Person *p);

    // Mitglieder
    void sendMailCurrent();

private:
    Ui::PersonalWindow *ui;
    ManagerPersonal *manager;

    CoreMainWindow *parentWindow;
    Person *aktuellePerson;
    QList<Person*> current;
    Status filter;
    QHash<Person*, QListWidgetItem*> personToItem;
    QSet<Category> anzeige;

    bool enabled; // Gibt an, ob das Formular aktiviert ist oder nicht, und ob Änderungen übernommen werden

    QList<Person*> getSortierteListe();

    void toggleFields(bool state);

    void setZeitenNachVeraenderung(Category cat, QString arg);

    void updateZeiten();
    void faerbeZelle(QTableWidgetItem *item, QString hintergrund, QString vordergrund = "black");
};
#endif // PERSONALWINDOW_H
