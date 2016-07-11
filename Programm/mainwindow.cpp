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

    fenster = new QMap<AActivity*, QMainWindow*>();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFahrtag(Fahrtag *f)
{
    if (fenster->contains(f)) {
        fenster->value(f)->show();
        fenster->value(f)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster->value(f)->raise();  // for MacOS
        fenster->value(f)->activateWindow(); // for Windows
    } else {
        FahrtagWindow *w = new FahrtagWindow(this, f);
        fenster->insert(f, w);
        w->show();
    }
}

void MainWindow::openActivity(Activity *a)
{
    if (fenster->contains(a)) {
        fenster->value(a)->show();
        fenster->value(a)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster->value(a)->raise();  // for MacOS
        fenster->value(a)->activateWindow(); // for Windows
    } else {
        ActivityWindow *w = new ActivityWindow(this, a);
        fenster->insert(a, w);
        w->show();
    }
}
