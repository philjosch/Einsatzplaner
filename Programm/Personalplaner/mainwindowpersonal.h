#ifndef MAINWINDOWPERSONAL_H
#define MAINWINDOWPERSONAL_H

#include "coremainwindow.h"
#include "manager.h"
#include "personwindow.h"

#include <QListWidget>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowPersonal; }
QT_END_NAMESPACE

class MainWindowPersonal : public CoreMainWindow
{
    Q_OBJECT

public:
    MainWindowPersonal(EplFile *file = new EplFile());
    ~MainWindowPersonal();

    static bool open(QString path);

protected slots:
    //** Vererbte Methoden von CoreMainWindow
    CoreMainWindow *handlerNew();
    void handlerOpen(QString path);

    //** Menueleiste
    // Bearbeiten
    void addPerson();
    void refresh();
    void editMinimumhours();
    void sendMailList();
    void editDues();

    // Export
    void exportMemberDetailMultiplePdf();
    void exportMemberDetailMultiplePrint();

    void exportMemberListPdf();
    void exportMemberListPrint();
    void exportMemberListCsv();

    void exportDuesRegularCsv();
    void exportDuesAdditionalCsv();

    //** Fenster
    void filterChanged(int index);
    void showPersFromTable(int row, int column);

protected:
    //** Hilfsmethoden
    QList<Person*> getSortierteListe();

    //** Modell

    //** View
    Ui::MainWindowPersonal *ui;

    //** Controller
    QList<Person*> current;
    Status filter;
    QSet<QString> anzeige;

private slots:
    void viewShowColumFromItem(QListWidgetItem *item);
};
#endif // MAINWINDOWPERSONAL_H
