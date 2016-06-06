#include "fahrtagwindow.h"
#include "ui_fahrtagwindow.h"

FahrtagWindow::FahrtagWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FahrtagWindow)
{
    ui->setupUi(this);
}

FahrtagWindow::~FahrtagWindow()
{
    delete ui;
}
