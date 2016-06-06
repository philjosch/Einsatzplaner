#include "personalwindow.h"
#include "ui_personalwindow.h"

PersonalWindow::PersonalWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PersonalWindow)
{
    ui->setupUi(this);
}

PersonalWindow::~PersonalWindow()
{
    delete ui;
}
