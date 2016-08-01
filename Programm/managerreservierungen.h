#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QFrame>

namespace Ui {
class ManagerReservierungen;
}

class ManagerReservierungen: public QFrame
{
    Q_OBJECT

public:
    explicit ManagerReservierungen(QWidget *parent = 0);
    ~ManagerReservierungen();

    QString getWagenreihung() const;
    void setWagenreihung(const QString &value);

private:
    Ui::ManagerReservierungen *ui;

protected:
    QString wagenreihung;
};

#endif // MANAGERRESERVIERUNGEN_H
