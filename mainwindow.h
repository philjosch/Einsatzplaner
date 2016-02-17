#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QListWidgetItem>
#include <QMainWindow>
#include "qfahrtag.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool saved;
    bool genommen;
    QListWidgetItem *aktuellerZug;
    QHash<QListWidgetItem*, QFahrtag*> Fahrtage;
    QList<QColor> Farben;

private slots:
    void setState(bool aktiv);
    void on_ButtonAdd_clicked();
    void on_ButtonNehmen_clicked();
    void bearbeiten(QListWidgetItem *item_x);
    void aktualisieren();
    void ZugLaden(QListWidgetItem *zug);
    void ZugSichern();
};

#endif // MAINWINDOW_H
