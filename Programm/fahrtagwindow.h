#ifndef FAHRTAGWINDOW_H
#define FAHRTAGWINDOW_H

#include <QMainWindow>
#include "fahrtag.h"
#include <QListWidgetItem>

namespace Ui {
class FahrtagWindow;
}

class FahrtagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FahrtagWindow(QWidget *parent, Fahrtag *f);
    ~FahrtagWindow();

private slots:
    void on_dateDate_dateChanged(const QDate &date);

    void on_comboArt_currentIndexChanged(int index);

    void on_textAnlass_textChanged();

    void on_checkWichtig_stateChanged(int arg1);

    void on_comboWagenreihung_currentTextChanged(const QString &arg1);

    void on_comboTimeTfH_currentTextChanged(const QString &arg1);

    void on_comboTimeTfM_currentTextChanged(const QString &arg1);

    void on_comboTimeZH_currentTextChanged(const QString &arg1);

    void on_comboTimeZM_currentTextChanged(const QString &arg1);

    void on_listTf_itemChanged(QListWidgetItem *item);

    void on_listZub_itemChanged(QListWidgetItem *item);

    void on_listZf_itemChanged(QListWidgetItem *item);

    void on_listService_itemChanged(QListWidgetItem *item);

    void on_textBemerkungen_textChanged();

    void on_plainBeschreibung_textChanged();

    void on_timeBeginn_timeChanged(const QTime &time);

    void on_timeEnde_timeChanged(const QTime &time);

    void on_checkBoxBenoetigt_stateChanged(int arg1);

private:
    Ui::FahrtagWindow *ui;
    Fahrtag *fahrtag;

    void loadData();
};

#endif // FAHRTAGWINDOW_H
