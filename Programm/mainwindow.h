#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "aactivity.h"
#include "fahrtag.h"
#include "fahrtagwindow.h"
#include "activity.h"
#include "activitywindow.h"
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

private:
    Ui::MainWindow *ui;
    QMap<AActivity*, QMainWindow*> *fenster;

public slots:
    void openFahrtag(Fahrtag *f);
    void openActivity(Activity *a);
};

#endif // MAINWINDOW_H
