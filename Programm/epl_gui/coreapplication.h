#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include "basics.h"
#include "coremainwindow.h"
#include "networking.h"

#include <QApplication>

class CoreApplication : public QApplication
{
    Q_OBJECT

public:
    CoreApplication(int &argc, char **argv);
    ~CoreApplication();
    static CoreApplication* generateApp(int argc, char *argv[], QString name, QString vers, bool deploy, bool debug, QString hash);

    bool generateWindow();

    bool event(QEvent *event);

    void checkVersion();

    void startAutoSave(int delay); // in Minuten
    void stopAutoSave();

    void openUnopenedFiles();

public slots:
    static void closeAllWindows();
    static void autoSaveWindows();

    static void oeffneDownloadSeite();

protected:
    QTimer *autoSaveTimer;

    static QString URL_DOWNLOAD;

    static QList<CoreMainWindow *> getCoreMainWindows();

private:
    QStringList unopenedFiles;
    bool openHandlerSet = false;
signals:
    void triggerOpen(QString);
};

#endif // COREAPPLICATION_H
