#include "personal.h"
#include "ui_personal.h"

Personal::Personal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Personal)
{
    ui->setupUi(this);
}

Personal::~Personal()
{
    delete ui;
}
