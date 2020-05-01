#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include "basics.h"
#include "networking.h"

#include <QApplication>

class CoreApplication : public QApplication
{
    Q_OBJECT

public:
    CoreApplication(int &argc, char **argv, Version version, bool devVersion);
    ~CoreApplication();

    bool getIsFirst() const;

    bool event(QEvent *event);

    void checkVersion();

    void startAutoSave(int delay); // in Minuten
    void stopAutoSave();

    static Version loadVersion();

    static QString loadNotes(Version v);
    static QString URL_DOWNLOAD;
    static QString URL_VERSION;
    static QString URL_NOTES;

    static bool DEVELOPER_MODE;
    static Version VERSION;

public slots:
    static void closeAllWindows();
    static void autoSaveWindows();

protected:
    QTimer *autoSaveTimer;
    bool isFirst;
};

#endif // COREAPPLICATION_H
