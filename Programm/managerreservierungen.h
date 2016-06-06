#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QFrame>

namespace Ui {
class ManagerReservierungen;
}

class ManagerReservierungen : public QFrame
{
    Q_OBJECT

public:
    explicit ManagerReservierungen(QWidget *parent = 0);
    ~ManagerReservierungen();

private:
    Ui::ManagerReservierungen *ui;
};

#endif // MANAGERRESERVIERUNGEN_H
