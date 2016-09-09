#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "aactivity.h"
#include "fahrtag.h"
#include "fahrtagwindow.h"
#include "activity.h"
#include "activitywindow.h"
#include "personalwindow.h"
#include <QMap>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static QString getFarbeZug(Fahrtag::Art cat);
    static QString getFarbeArbeit();
    static QString getFarbe(AActivity *a);

private:
    Ui::MainWindow *ui;
    QMap<AActivity*, QMainWindow*> *fenster;

    PersonalWindow *personalfenster;

public slots:
    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);
private slots:
    void on_buttonPersonal_clicked();
    void on_buttonExport_clicked();
};

#endif // MAINWINDOW_H
