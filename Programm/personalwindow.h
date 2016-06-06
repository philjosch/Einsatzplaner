#ifndef PERSONALWINDOW_H
#define PERSONALWINDOW_H

#include <QMainWindow>

namespace Ui {
class PersonalWindow;
}

class PersonalWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PersonalWindow(QWidget *parent = 0);
    ~PersonalWindow();

private:
    Ui::PersonalWindow *ui;
};

#endif // PERSONALWINDOW_H
