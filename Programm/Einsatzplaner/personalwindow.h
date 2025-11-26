#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include "managerpersonal.h"

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <coremainwindow.h>
#include <QTreeWidgetItem>

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

    void updateTableBasedOnCategorySelection(QTreeWidgetItem *item, int column);

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

    void persSetAdditionalAnzahl(double arg1);
    void persSetAdditionalKilometer(double arg1);


    void showPerson(Person *p);
    void tableEntryDoubleClicked(int row, int column);

    // Mitglieder
    void sendMailCurrent();

private:
    Ui::PersonalWindow *ui;
    QComboBox *comboAnzeige;
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
    void toggleShowCategory(Category cat, bool show);

    void setZeitenNachVeraenderung(Category cat, QString arg);

    void updateZeiten();
    void faerbeZelle(QTableWidgetItem *item, QString hintergrund, QString vordergrund = "black");
};
#endif // PERSONALWINDOW_H
