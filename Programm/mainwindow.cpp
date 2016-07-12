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

QString MainWindow::getFarbeZug(Fahrtag::Art cat)
{
    switch (cat) {
    case Fahrtag::Museumszug:           return "#fffff0f"; // Museumszug
    case Fahrtag::Sonderzug:            return "#ffc789"; // Sonderzug
    case Fahrtag::Nikolauszug:          return "#b7a0e3"; // Nikolausfahrt
    case Fahrtag::ELFundMuseumszug:     return "#afcad6"; // Museumszug mit ELF
    case Fahrtag::Schnupperkurs:        return "#99c9de"; // ELF-Schnupperkurs
    case Fahrtag::Bahnhofsfest:         return "#d3f7a7"; // Bahnhofsfest
//    case ???:     return "#fff5ac"; // Ausbildung
    case Fahrtag::Sonstiges:            return "#f2a4c3"; // Sonstiges
    default:                            return "#dddddd";
    }
}

QString MainWindow::getFarbeArbeit()
{
    return "#cccccc";
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
