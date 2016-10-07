#include "arbeitseinatz.h"
#include "ui_arbeitseinatz.h"

Arbeitseinatz::Arbeitseinatz(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Arbeitseinatz)
{
    ui->setupUi(this);
}

Arbeitseinatz::~Arbeitseinatz()
{
    delete ui;
}
