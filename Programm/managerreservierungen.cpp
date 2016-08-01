#include "managerreservierungen.h"
#include "ui_managerreservierungen.h"

ManagerReservierungen::ManagerReservierungen(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ManagerReservierungen)
{
    ui->setupUi(this);
    wagenreihung = "309, 221, 204, 208";
}

ManagerReservierungen::~ManagerReservierungen()
{
    delete ui;
}

QString ManagerReservierungen::getWagenreihung() const
{
    return wagenreihung;
}

void ManagerReservierungen::setWagenreihung(const QString &value)
{
    wagenreihung = value;
}
