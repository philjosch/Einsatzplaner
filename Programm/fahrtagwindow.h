#ifndef FAHRTAGWINDOW_H
#define FAHRTAGWINDOW_H

#include <QMainWindow>

namespace Ui {
class FahrtagWindow;
}

class FahrtagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FahrtagWindow(QWidget *parent = 0);
    ~FahrtagWindow();

private:
    Ui::FahrtagWindow *ui;
};

#endif // FAHRTAGWINDOW_H
