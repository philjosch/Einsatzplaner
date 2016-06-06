#include "managerreservierungen.h"
#include "ui_managerreservierungen.h"

ManagerReservierungen::ManagerReservierungen(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ManagerReservierungen)
{
    ui->setupUi(this);
}

ManagerReservierungen::~ManagerReservierungen()
{
    delete ui;
}
