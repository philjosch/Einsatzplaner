#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "personal.h"
#include "arbeitseinatz.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    Personal *p  = new Personal(this);
    p->show();
    Arbeitseinatz *a = new Arbeitseinatz(this);
    a->show();
}
