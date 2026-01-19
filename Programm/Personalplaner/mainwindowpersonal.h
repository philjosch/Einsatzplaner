#ifndef MAINWINDOWPERSONAL_H
#define MAINWINDOWPERSONAL_H

#include "coremainwindow.h"

#include <QListWidget>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>

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
    void comboAnzeigeChanged(int newIndex);
    void showPersFromTable(QModelIndex index);
    void categorySelectionChanged(QTreeWidgetItem *item, int column);

protected:
    //** Hilfsmethoden
    QList<Person*> getSortierteListe();

    //** Modell

    //** View
    Ui::MainWindowPersonal *ui;
    QComboBox *comboAnzeige;

    //** Controller

};
#endif // MAINWINDOWPERSONAL_H
