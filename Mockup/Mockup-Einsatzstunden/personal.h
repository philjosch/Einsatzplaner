#ifndef PERSONAL_H
#define PERSONAL_H

#include <QMainWindow>

namespace Ui {
class Personal;
}

class Personal : public QMainWindow
{
    Q_OBJECT

public:
    explicit Personal(QWidget *parent = 0);
    ~Personal();

private:
    Ui::Personal *ui;
};

#endif // PERSONAL_H
