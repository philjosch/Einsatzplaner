#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "export.h"
#include "exportgesamt.h"
#include "fileio.h"
#include "coreapplication.h"
#include "preferencesdialog.h"
#include "filesettings.h"

#include <QMessageBox>


void MainWindow::wheelEvent(QWheelEvent *event)
{
//    int scrollAmount = 30;
//    scrollOffSet += event->angleDelta().y();
//    ui->dateSelector->setDate(ui->dateSelector->date().addDays(- scrollOffSet / scrollAmount));
//    scrollOffSet %= scrollAmount;
}

void MainWindow::showDate(QDate date)
{
    int year = date.year();
    int month = date.month();
    int day;
    int wochentag = QDate(year, month, 1).dayOfWeek();
    if (date.month() == 1) {
        year -= 1;
        month = 12;
    } else {
        month -= 1;
    }
    day = QDate(year, month, 1).daysInMonth();
    QDate start = QDate(year, month, day).addDays(2-wochentag);

    // Eintragen der Wochennummern
    ui->number1_1->setText(QString::number(start.addDays(0).weekNumber()));
    ui->number1_2->setText(QString::number(start.addDays(7).weekNumber()));
    ui->number1_3->setText(QString::number(start.addDays(14).weekNumber()));
    ui->number1_4->setText(QString::number(start.addDays(21).weekNumber()));
    ui->number1_5->setText(QString::number(start.addDays(28).weekNumber()));
    ui->number1_6->setText(QString::number(start.addDays(35).weekNumber()));


    // Einstellen der einzelnen Tage
    for (int i = 0; i < wochentag-1; i++) {
        tage.at(i)->show(start);
        tage.at(i)->setGray(true);
        start = start.addDays(1);
    }
    day = start.daysInMonth();

    for(int i = wochentag-1; i < (wochentag + day - 1); i++) {
        tage.at(i)->show(start);
        tage.at(i)->setGray(false);
        start = start.addDays(1);
    }
    for(int i = (wochentag + day - 1); i < 42; i++) {
        tage.at(i)->show(start);
        tage.at(i)->setGray(true);
        start = start.addDays(1);
    }

    foreach (CalendarDay *d, tage) {
        d->clear();
    }
    foreach (AActivity *a, manager->getActivities()) {
        int pos = getPosInCalendar(a->getDatum());
        if (pos != -1) {
            tage.at(pos)->insert(a);
        }
    }
}

void MainWindow::newFahrtag(QDate d)
{
    // Anlegen des Fahrtags
    Fahrtag *f = manager->newFahrtag(d);
    connect(f, SIGNAL(changed(AActivity*, QDate)), this, SLOT(activityChanged(AActivity*, QDate)));
    connect(f, SIGNAL(del(AActivity*)), this, SLOT(removeActivity(AActivity*)));

    // Einfügen in Seitenliste und Kalender
    insert(f);
    activityChanged(f);

    // Anzeigen eines neuen Fensters
    openFahrtag(f);
    unsave();
}

void MainWindow::newActivity(QDate d)
{
    // Anlegen der Aktivität
    Activity *a = manager->newActivity(d);
    connect(a, SIGNAL(changed(AActivity*, QDate)), this, SLOT(activityChanged(AActivity*, QDate)));
    connect(a, SIGNAL(del(AActivity*)), this, SLOT(removeActivity(AActivity*)));

    // Einfügen in die Seitenleiste
    insert(a);
    activityChanged(a);

    // Anzeigen einen neuen Fensters
    openActivity(a);
    unsave();
}

bool MainWindow::removeActivity(AActivity *a)
{
    ui->listWidget->takeItem(ui->listWidget->row(listitem.value(a)));
    itemToList.remove(listitem.value(a));
    listitem.remove(a);
    tage.at(getPosInCalendar(a->getDatum()))->remove(a);
    bool ret = manager->removeActivity(a);
    unsave();
    if (fenster.contains(a))
        fenster.remove(a);
    return ret;
}

void MainWindow::activityChanged(AActivity *a, QDate oldDate)
{
    manager->activityChanged(a);
    int oldPos = -1;
    if (oldDate.isValid())
        oldPos = getPosInCalendar(oldDate);

    if (oldPos < 42 && oldPos >= 0) {
        tage.at(oldPos)->remove(a);
    }

    // Richtiges Anzeigen im Kalender
    int pos = getPosInCalendar(a->getDatum());
    if (pos < 42 && pos >= 0) {
        tage.at(pos)->insert(a);
    }

    // Richtiges Anzeigen in der Übersichts liste
    setListItem(listitem.value(a), a);

    // Prüfen, ob das element weitgenug vorne (oben) ist
    int i = ui->listWidget->row(listitem.value(a));
    AActivity *ref;
    while (i > 0) {
        ref = itemToList.value(ui->listWidget->item(i-1));
        if (*a < *ref) {
            ui->listWidget->insertItem(i-1, ui->listWidget->takeItem(i));
            i--;
        } else {
            break;
        }
    }

    // Prüfen, ob das element weit genug hinten (unten) ist
    while (i < ui->listWidget->count()-1) {
        ref = itemToList.value(ui->listWidget->item(i+1));
        if (*ref < *a) {
            ui->listWidget->insertItem(i, ui->listWidget->takeItem(i+1));
            i++;
        } else {
            break;
        }
    }
    unsave();
}

void MainWindow::itemInCalendarDayClicked(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        openFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        openActivity(c);
    }
}

int MainWindow::getPosInCalendar(QDate date)
{
    QDate ref = QDate(ui->dateSelector->date().year(), ui->dateSelector->date().month(), 1);
    // Eintrag auch anzeigen, wenn er im vorherigen Monat liegt
    if (date.month() == ref.month() && date.year() == ref.year()) {
        int year = ui->dateSelector->date().year();
        int month = ui->dateSelector->date().month();
        int wochentag = QDate(year, month, 1).dayOfWeek();

        return wochentag - 1 + date.day() -1;
    } else if (date.addMonths(1).month() == ref.month() && date.addMonths(1).year() == ref.year()) {
        if (date.addDays(7) > ref && date.dayOfWeek() < ref.dayOfWeek()) {
            return date.dayOfWeek() - 1;
        }
        return -1;
    } else if (date.month() == ref.addMonths(1).month() && date.year() == ref.addMonths(1).year()) {
        if (date.day()+ref.dayOfWeek()+ref.daysInMonth()-2 < 42) {
            return date.day()+ref.dayOfWeek()+ref.daysInMonth()-2;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

void MainWindow::insert(AActivity *a)
{
    // Einfügen in die Seitenliste
    ui->listWidget->insertItem(ui->listWidget->count(), a->getListString());
    QListWidgetItem *i = ui->listWidget->item(ui->listWidget->count()-1);
    listitem.insert(a, i);
    itemToList.insert(i, a);

    // Einfügen in den Kalender mit allem drum und dran
    int pos = getPosInCalendar(a->getDatum());
    if (pos != -1) {
        tage.at(pos)->insert(a);
    }
}

void MainWindow::setListItem(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getListString());
    i->setToolTip(a->getAnlass());
    i->setWhatsThis(a->getAnlass());
    i->setBackground(QBrush(QColor(getFarbe(a))));
}



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Modell
    manager = new Manager();
    filePath = "";
    saved = true;
    // Views
    personalfenster = new PersonalWindow(this, manager->getPersonal());
    settings = new ManagerFileSettings();
    exportDialog = new ExportGesamt(manager, settings, this);
    // Controller
    listitem = QMap<AActivity*, QListWidgetItem*>();
    itemToList = QMap<QListWidgetItem*, AActivity*>();
    fenster = QMap<AActivity*, QMainWindow*>();
    scrollOffSet = 0;

    setWindowTitle(tr("Neues Dokument – Übersicht"));
    setWindowModified(false);
    setWindowIcon(QApplication::windowIcon());

    connect(personalfenster, SIGNAL(changed()), this, SLOT(unsave()));
    connect(ui->buttonAddActivity, SIGNAL(clicked(bool)), this, SLOT(newActivity()));
    connect(ui->buttonAddFahrtag, SIGNAL(clicked(bool)), this, SLOT(newFahrtag()));
    connect(ui->dateSelector, SIGNAL(dateChanged(QDate)), this, SLOT(showDate(QDate)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onItemInListClicked(QListWidgetItem*)));

    // Setup fuer die Darstellung des Kalenders
    tage = QList<CalendarDay*>();
    tage.append(ui->day1_1);    tage.append(ui->day2_1);    tage.append(ui->day3_1);    tage.append(ui->day4_1);
    tage.append(ui->day5_1);    tage.append(ui->day6_1);    tage.append(ui->day7_1);
    tage.append(ui->day1_2);    tage.append(ui->day2_2);    tage.append(ui->day3_2);    tage.append(ui->day4_2);
    tage.append(ui->day5_2);    tage.append(ui->day6_2);    tage.append(ui->day7_2);
    tage.append(ui->day1_3);    tage.append(ui->day2_3);    tage.append(ui->day3_3);    tage.append(ui->day4_3);
    tage.append(ui->day5_3);    tage.append(ui->day6_3);    tage.append(ui->day7_3);
    tage.append(ui->day1_4);    tage.append(ui->day2_4);    tage.append(ui->day3_4);    tage.append(ui->day4_4);
    tage.append(ui->day5_4);    tage.append(ui->day6_4);    tage.append(ui->day7_4);
    tage.append(ui->day1_5);    tage.append(ui->day2_5);    tage.append(ui->day3_5);    tage.append(ui->day4_5);
    tage.append(ui->day5_5);    tage.append(ui->day6_5);    tage.append(ui->day7_5);
    tage.append(ui->day1_6);    tage.append(ui->day2_6);    tage.append(ui->day3_6);    tage.append(ui->day4_6);
    tage.append(ui->day5_6);    tage.append(ui->day6_6);    tage.append(ui->day7_6);
    for(CalendarDay *c: tage) {
        connect(c, SIGNAL(clickedItem(AActivity*)), this, SLOT(itemInCalendarDayClicked(AActivity*)));
        connect(c, SIGNAL(addActivity(QDate)), this, SLOT(newActivity(QDate)));
    }
    on_buttonToday_clicked();
}

MainWindow::~MainWindow()
{
    this->close();
    delete ui;
}

void MainWindow::open(QString path)
{
    MainWindow *newOpen = new MainWindow();
    if (newOpen->openFile(path)) {
        newOpen->show();
    } else {
        newOpen->close();
    }
}

void MainWindow::openFahrtag(Fahrtag *f)
{
    if (fenster.contains(f)) {
        fenster.value(f)->show();
        fenster.value(f)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster.value(f)->raise();  // for MacOS
        fenster.value(f)->activateWindow(); // for Windows
    } else {
        FahrtagWindow *w = new FahrtagWindow(this, f);
        w->setWindowFilePath(filePath);
        fenster.insert(f, w);
        w->show();
    }
}

void MainWindow::openActivity(Activity *a)
{
    if (fenster.contains(a)) {
        fenster.value(a)->show();
        fenster.value(a)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster.value(a)->raise();  // for MacOS
        fenster.value(a)->activateWindow(); // for Windows
    } else {
        ActivityWindow *w = new ActivityWindow(this, a);
        w->setWindowFilePath(filePath);
        fenster.insert(a, w);
        w->show();
    }
}

void MainWindow::unsave()
{
    this->saved = false;
    setWindowModified(true);
}

void MainWindow::autoSave()
{
    if (filePath == "") return;
    if (saved) return;
    saveToPath(filePath+".autosave.ako", false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool toClose = false;
    if (!saved) {
        QMessageBox::StandardButton answ = QMessageBox::question(this, tr("Ungesicherte Änderungen"),
                                                                 tr("Möchten Sie die Datei wirklich schließen?\nIhre ungesicherten Änderungen gehen dann verloren!"),
                                                                 QMessageBox::Close|QMessageBox::Cancel|QMessageBox::Save, QMessageBox::Save);
        if (answ == QMessageBox::Save) {
            on_actionSave_triggered();
            if (saved) {
                toClose = close();
            }
        } else if (answ == QMessageBox::Cancel) {
            toClose = false;
        } else if (answ == QMessageBox::Close) {
            toClose = close();
        }
    } else {
        toClose = true;
    }

    if (toClose) {
        personalfenster->close();
        for(QMainWindow *m: fenster.values()) {
            m->close();
        }
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }
    }

    if (toClose) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::saveToPath(QString path, bool showInMenu)
{
    QJsonObject calendarJSON = manager->toJson();
    calendarJSON.insert("currentDate", ui->dateSelector->date().toString("yyyy-MM-dd"));

    QJsonObject viewJSON;
    viewJSON.insert("xMain", this->x());
    viewJSON.insert("yMain", this->y());
    viewJSON.insert("widthMain", this->width());
    viewJSON.insert("heightMain", this->height());
    viewJSON.insert("xPersonal", personalfenster->x());
    viewJSON.insert("yPersonal", personalfenster->y());
    viewJSON.insert("widthPersonal", personalfenster->width());
    viewJSON.insert("heightPersonal", personalfenster->height());
/*    viewJSON.insert("showPersonal", false); CURRENTLY NOT USED*/

    QJsonObject generalJSON;
    generalJSON.insert("version", CoreApplication::getAktuelleVersion().toStringShort());

    QJsonObject object;
    object.insert("calendar", calendarJSON);
    object.insert("view", viewJSON);
    object.insert("settings", settings->toJson());
    object.insert("general", generalJSON);

    return FileIO::saveJsonToFile(path, object, showInMenu);
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
    exportDialog->hardReload();
    exportDialog->exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog *dialog = new PreferencesDialog();
    dialog->setWindowFilePath("");
    dialog->show();
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAboutApp_triggered()
{
    QMessageBox::about(this, tr("Über Einsatzplaner"), "<h1>Einsatzplaner</h1>"+QCoreApplication::applicationVersion()+(CoreApplication::isDeveloperVersion()?" (dev) ":"")+ "<br/>2016-2020 Philipp Schepper");
}

void MainWindow::on_actionQuit_triggered()
{
    CoreApplication::closeAllWindows();
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow *w = new MainWindow();
    w->show();
}

void MainWindow::on_actionOpen_triggered()
{
    QString file = FileIO::getFilePathOpen(this, tr("AkO-Dateien (*.ako)"));
    if (file != "") open(file);
}

void MainWindow::on_menuRecentlyused_aboutToShow()
{
    QStringList list = FileIO::getLastUsed();

    if (list.length() > 0) {
        QList<QAction*> actions = ui->menuRecentlyused->actions();
        for (int i = 2; i < actions.length(); i++) {
            ui->menuRecentlyused->removeAction(actions[i]);
        }
        ui->actionClear->setEnabled(true);
        foreach (QString entry, list) {
            QAction *a = new QAction(entry, this);
            connect(a, SIGNAL(triggered(bool)), this, SLOT(open()));
            ui->menuRecentlyused->insertAction(nullptr, a);
        }
    }
}

void MainWindow::open()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) open(action->text());
}

void MainWindow::on_actionClear_triggered()
{
    FileIO::clearLastUsed();
    QList<QAction*> actions = ui->menuRecentlyused->actions();
    for (int i = 2; i < actions.length(); i++) {
        ui->menuRecentlyused->removeAction(actions[i]);
    }
    ui->actionClear->setEnabled(false);
}

void MainWindow::on_actionSave_triggered()
{
    if (filePath == "") {
        on_actionSaveas_triggered();
        return;
    }

    bool erfolg = saveToPath(filePath);
    if (erfolg) {
        saved = true;
        setWindowModified(false);
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }

        int result = Export::autoUploadToServer(settings, manager);
        if (result == 0)
            ui->statusBar->showMessage(tr("Datei konnte nicht hochgeladen werden!"), 5000);
        else if (result > 0)
            ui->statusBar->showMessage(tr("Datei wurde erfolgreich hochgeladen!"), 5000);

    } else {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
        filePath = "";
    }
}

bool MainWindow::openFile(QString path)
{
    // Daten aus Datei laden
    QJsonObject object = FileIO::getJsonFromFile(path);
    if (object == QJsonObject()) return false;

    // Prüfen, ob Version kompatibel ist
    QJsonObject generalJSON = object.value("general").toObject();
    CoreApplication::Version version = CoreApplication::Version::stringToVersion(generalJSON.value("version").toString());
    if (version > (CoreApplication::getAktuelleVersion()) || CoreApplication::Version{-1,-1,-1} == version) {
        QMessageBox::warning(this, tr("Nicht kompatibel"),
                             tr("Die Datei kann nicht mit dieser Version geöffnet werden.\n"
                                "Das Dokument benötigt mindestens Version %1.\n"
                                "Die aktuellste Version finden Sie auf der Webseite des Programms.\n"
                                "Bei weiteren Fragen wenden Sie sich bitte an den Support.").arg(version.toString()));
        return false;
    }
    setWindowTitle(tr("Übersicht"));
    filePath = path;
    setWindowFilePath(filePath);
    personalfenster->setWindowFilePath(filePath);

    // Daten in Manager laden und darstellen lassen
    QJsonObject calendarJSON = object.value("calendar").toObject();
    QJsonObject personalJSON;
    if (object.contains("personal")) {
        personalJSON = object.value("personal").toObject();
    } else {
        personalJSON = calendarJSON.value("personal").toObject();
    }
    // Daten in den Manager laden und die Logik herstellen
    QJsonObject c = calendarJSON;
    QJsonObject p = personalJSON;
    manager->fromJson(c, p);
    // Alle aktivitäten in die seitenleiste eintragen
    for(AActivity *a: manager->getActivities()) {
        QListWidgetItem *i = new QListWidgetItem("");
        if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
            connect(f, SIGNAL(changed(AActivity*, QDate)), this, SLOT(activityChanged(AActivity*, QDate)));
            connect(f, SIGNAL(del(AActivity*)), this, SLOT(removeActivity(AActivity*)));
        } else {
            Activity *a_cast = dynamic_cast<Activity*>(a);
            connect(a_cast, SIGNAL(changed(AActivity*,QDate)), this, SLOT(activityChanged(AActivity*,QDate)));
            connect(a_cast, SIGNAL(del(AActivity*)), this, SLOT(removeActivity(AActivity*)));
        }
        setListItem(i, a);
        ui->listWidget->insertItem(ui->listWidget->count(), i);
        listitem.insert(a, i);
        itemToList.insert(i, a);
    }
    // an das gespeicherte Datum gehen
    ui->dateSelector->setDate(QDate::fromString(c.value("currentDate").toString(), "yyyy-MM-dd"));
    ui->dateSelector->repaint();
    showDate(ui->dateSelector->date());


    personalfenster->loadData();

    //- Hier prüfen, ob Personalfenster angezeigt wurde und wiederherstellen der Fensterpositionen
    QJsonObject viewJSON = object.value("view").toObject();
    int x = viewJSON.value("xMain").toInt();
    int y = viewJSON.value("yMain").toInt();
    int w = viewJSON.value("widthMain").toInt();
    int h = viewJSON.value("heightMain").toInt();
    this->setGeometry(x, y, w, h);
    // Personalfenster
    x = viewJSON.value("xPersonal").toInt();
    y = viewJSON.value("yPersonal").toInt();
    w = viewJSON.value("widthPersonal").toInt();
    h = viewJSON.value("heightPersonal").toInt();
    personalfenster->setGeometry(x, y, w, h);
    if (viewJSON.value("showPersonal").toBool(false)) {
        personalfenster->show();
        personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        personalfenster->raise();
        personalfenster->activateWindow();
    } else {
        personalfenster->hide();
    }

    settings->fromJson(object.value("settings").toObject());
    return true;
}


void MainWindow::on_actionSaveas_triggered()
{
    QString newPath = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (newPath != "") {
        filePath = newPath;
        QJsonObject o;
        bool erfolg = FileIO::saveJsonToFile(filePath, o);
        if (! erfolg) {
            QMessageBox::information(this, tr("Fehler"), tr("Das speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
            filePath = "";
            return;
        }
        setWindowTitle(tr("Übersicht"));
        setWindowFilePath(filePath);
        personalfenster->setWindowFilePath(filePath);
        for(QMainWindow *mw: fenster.values()) {
            mw->setWindowFilePath(filePath);
        }
        on_actionSave_triggered();
    }
}

void MainWindow::on_actionSavePersonal_triggered()
{
    QString path = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (path == "") {
        return;
    }

    QJsonObject calendarJSON = manager->personalToJson();
    calendarJSON.insert("currentDate", ui->dateSelector->date().toString("yyyy-MM-dd"));


    QJsonObject viewJSON;
    viewJSON.insert("xMain", this->x());
    viewJSON.insert("yMain", this->y());
    viewJSON.insert("widthMain", this->width());
    viewJSON.insert("heightMain", this->height());
    viewJSON.insert("xPersonal", personalfenster->x());
    viewJSON.insert("yPersonal", personalfenster->y());
    viewJSON.insert("widthPersonal", personalfenster->width());
    viewJSON.insert("heightPersonal", personalfenster->height());
/*    viewJSON.insert("showPersonal", false); CURRENTLY NOT USED*/

    QJsonObject generalJSON;
    generalJSON.insert("version", CoreApplication::getAktuelleVersion().toStringShort());

    QJsonObject object;
    object.insert("calendar", calendarJSON);
    object.insert("view", viewJSON);
    object.insert("settings", settings->toJson());
    object.insert("general", generalJSON);

    if (! FileIO::saveJsonToFile(path, object)) {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
    }
}

void MainWindow::on_actionSettings_triggered()
{
    FileSettings s(this, settings);
    if (s.exec() == QDialog::Accepted) {
        s.getSettings(settings);
        emit unsave();

        // Update the settings
    }
}

bool MainWindow::on_actionClose_triggered()
{
    return close();
}

void MainWindow::on_buttonPrev_clicked()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(-1));
    ui->dateSelector->repaint();
}

void MainWindow::on_buttonNext_clicked()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(1));
    ui->dateSelector->repaint();
}

void MainWindow::on_buttonToday_clicked()
{
    ui->dateSelector->setDate(QDate::currentDate());
    ui->dateSelector->repaint();
}

void MainWindow::on_buttonDelete_clicked()
{
    QList<QListWidgetItem*> selected = ui->listWidget->selectedItems();
    if (! selected.isEmpty()) {
        if (QMessageBox::question(this, tr("Wirklich löschen?"), tr("Möchten Sie die ausgewählten Aktivitäten wirklich unwiderruflich löschen?")) == QMessageBox::Yes) {
            bool ok = true;
            for(QListWidgetItem *item: selected) {
                ok = ok && removeActivity(itemToList.value(item));
            }
        }
    }
}

void MainWindow::onItemInListClicked(QListWidgetItem *item)
{
    AActivity *a = itemToList.value(item);
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        openFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        openActivity(c);
    }
}
