#ifndef ACTIVITYWINDOW_H
#define ACTIVITYWINDOW_H

#include <QMainWindow>

namespace Ui {
class ActivityWindow;
}

class ActivityWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ActivityWindow(QWidget *parent = 0);
    ~ActivityWindow();

private slots:
    void on_buttonInsert_clicked();

    void on_buttonRemove_clicked();

private:
    Ui::ActivityWindow *ui;
};

#endif // ACTIVITYWINDOW_H
