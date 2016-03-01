#ifndef PLANERFAHRTAGE_H
#define PLANERFAHRTAGE_H
#include <QListWidgetItem>
#include <QMainWindow>
#include "qfahrtag.h"
#include <QListWidgetItem>

namespace Ui {
class PlanerFahrtage;
}

class PlanerFahrtage : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlanerFahrtage(QWidget *parent = 0);
    ~PlanerFahrtage();

private:
    Ui::PlanerFahrtage *ui;
    bool saved;
    bool uebernehmen;
    QListWidgetItem *aktuellerZug;
    QHash<QListWidgetItem*, QFahrtag*> Fahrtage;
    QList<QColor> Farben;

private slots:
    void setState(bool aktiv);
    void on_ButtonAdd_clicked();
    void createEmptyForm();
//    void bearbeiten(QListWidgetItem *zug);
    void aktualisieren();
    void ZugLaden(QListWidgetItem *zug);
    void ZugSichern();
    void deleteSelectedRowFromListWidget(QListWidget* curr);
    void addEmptyRowToListWidget(QListWidget* curr);

    // Aktionen für das Hinzufügen und löschen von Namen bei Tf, Zf, Zub und Service
    void on_toolButtonListTfDelete_clicked();
    void on_toolButtonListTfAdd_clicked();
    void on_toolButtonListZfDelete_clicked();
    void on_toolButtonListZfAdd_clicked();
    void on_toolButtonListZubDelete_clicked();
    void on_toolButtonListZubAdd_clicked();
    void on_toolButtonListServiceDelete_clicked();
    void on_toolButtonListServiceAdd_clicked();

    void on_toolButtonTableReservierungenDelete_clicked();
    void on_toolButtonTableReservierungenAdd_clicked();

    void resizeEvent(QResizeEvent *event);
    void changeSizeOfTableReservierungen();
    void on_comboTimeTfH_currentIndexChanged(int index);
    void on_comboTimeTfM_currentTextChanged(QString string);
    void on_comboTimeZH_currentIndexChanged(int index);
    void on_comboTimeZM_currentTextChanged(QString string);
    void on_comboArt_currentIndexChanged(int index);
    void on_comboWagenreihung_currentTextChanged(QString string);
    void on_dateZug_dateChanged(const QDate &date);
    void on_checkWichtig_stateChanged(int arg1);
    void on_textAnlass_textChanged();
    void on_listTf_itemChanged(QListWidgetItem *item);
    void on_listZf_itemChanged(QListWidgetItem *item);
    void on_listZub_itemChanged(QListWidgetItem *item);
    void on_listService_itemChanged(QListWidgetItem *item);
    void on_tableReservierungen_cellChanged(int row, int column);
    void on_ListeZuege_itemDoubleClicked(QListWidgetItem *item);
    void on_ListeZuege_itemClicked(QListWidgetItem *item);
};

#endif // PLANERFAHRTAGE_H
