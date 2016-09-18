#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calendar.h"
#include <QMessageBox>
#include "exportgesamt.h"
#include <QSettings>
#include "fileio.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->calendar, SIGNAL(showFahrtag(Fahrtag*)), this, SLOT(openFahrtag(Fahrtag*)));
    connect(ui->calendar, SIGNAL(showActivity(Activity*)), this, SLOT(openActivity(Activity*)));
    connect(ui->actionNeuer_Arbeitseinsatz, SIGNAL(triggered(bool)), ui->calendar, SLOT(newActivity()));
    connect(ui->actionNeuer_Fahrtag, SIGNAL(triggered(bool)), ui->calendar, SLOT(newFahrtag()));

    fenster = new QMap<AActivity*, QMainWindow*>();
    ui->calendar->setPersonal(new ManagerPersonal());

    setWindowTitle("Neues Dokument");
    filePath = "";
    saved = true;
    setWindowModified(false);

//    QSettings settings;
    personalfenster = new PersonalWindow(this, ui->calendar->getPersonal());
}

MainWindow::~MainWindow()
{/*
    QSettings settings;
    settings.setValue("window/main/x", this->x());
    settings.setValue("window/main/y", this->y());
    settings.setValue("window/main/width", this->width());
    settings.setValue("window/main/height", this->height());*/
    delete ui;
}

QString MainWindow::getFarbeZug(Fahrtag::Art cat)
{
/*
    case Fahrtag::Museumszug:           return "#ffffff"; // Museumszug
    case Fahrtag::Sonderzug:            return "#FFEBD4"; // Sonderzug
    case Fahrtag::Gesellschaftssonderzug: return "ffc789";
    case Fahrtag::Nikolauszug:          return "#b7a0e3"; // Nikolausfahrt
    case Fahrtag::ELFundMuseumszug:     return "#C8E6F3"; // Museumszug mit ELF
    case Fahrtag::Schnupperkurs:        return "#99c9de"; // ELF-Schnupperkurs
    case Fahrtag::Bahnhofsfest:         return "#d3f7a7"; // Bahnhofsfest
    case Fahrtag::Sonstiges:            return "#f2a4c3"; // Sonstiges
    }*/

    switch (cat) {
    case Fahrtag::Museumszug:           return "#ffffff"; // Museumszug
    case Fahrtag::Sonderzug:            return "#FFE8D9"; // Sonderzug -
    case Fahrtag::Gesellschaftssonderzug: return "#ffbc90"; // Gesellschaft -
    case Fahrtag::Nikolauszug:          return "#e481d1"; // Nikolausfahrt -
    case Fahrtag::ELFundMuseumszug:     return "#918fe3"; // Museumszug mit ELF -
    case Fahrtag::Schnupperkurs:        return "#E7E7FD"; // ELF-Schnupperkurs -
    case Fahrtag::Bahnhofsfest:         return "#80e3b1"; // Bahnhofsfest
    case Fahrtag::Sonstiges:            return "#ffeb90"; // Sonstiges
    default:                            return "#dddddd";
    }
}

QString MainWindow::getFarbeArbeit()
{
    return "#CCBEBE";
}

QString MainWindow::getFarbe(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a))
        return getFarbeZug(f->getArt());
    else
        return getFarbeArbeit();
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

void MainWindow::openFile(QString filePath)
{
    // Daten aus Datei laden
    QJsonObject *object = FileIO::getJsonFromFile(filePath);
    // Daten in Manager laden

    // Gui anpassen und neuladen
}

void MainWindow::on_buttonPersonal_clicked()
{
    personalfenster->show();
    personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    personalfenster->raise();
    personalfenster->activateWindow();
}

void MainWindow::on_buttonExport_clicked()
{
    ExportGesamt(ui->calendar, this).exec();
}

void MainWindow::on_actionPreferences_triggered()
{

}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAboutApp_triggered()
{
    QMessageBox::about(this, "Über Einsatzplaner", "Einsatzplaner "+QCoreApplication::applicationVersion()+"\n© 2016 by Philipp Schepper");
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow *w = new MainWindow();
    w->show();
}

void MainWindow::on_actionOpen_triggered()
{
    QString file = FileIO::getFilePathOpen(this);
    if (file != "") {
        MainWindow *newOpen = new MainWindow();
        newOpen->openFile(file);
        newOpen->show();
    }
}

void MainWindow::on_actionSave_triggered()
{

}

void MainWindow::on_actionSaveas_triggered()
{

}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}
