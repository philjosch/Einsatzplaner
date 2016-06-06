#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"calendar.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->calendar, SIGNAL(showFahrtag(Fahrtag*)), this, SLOT(openFahrtag(Fahrtag*)));
    connect(ui->calendar, SIGNAL(showActivity(Activity*)), this, SLOT(openActivity(Activity*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFahrtag(Fahrtag *f)
{
    FahrtagWindow *w = new FahrtagWindow(this);
    w->show();
}

void MainWindow::openActivity(Activity *a)
{
    ActivityWindow *w = new ActivityWindow(this);
    w->show();
}
