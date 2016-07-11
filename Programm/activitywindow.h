#ifndef ACTIVITYWINDOW_H
#define ACTIVITYWINDOW_H

#include <QMainWindow>
#include "activity.h"

namespace Ui {
class ActivityWindow;
}

class ActivityWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActivityWindow(QWidget *parent, Activity *a);
    ~ActivityWindow();

private slots:
    void on_buttonInsert_clicked();

    void on_buttonRemove_clicked();

private:
    Ui::ActivityWindow *ui;
    Activity *activity;
};

#endif // ACTIVITYWINDOW_H
