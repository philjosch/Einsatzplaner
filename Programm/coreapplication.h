#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include <QApplication>

class CoreApplication : public QApplication
{
    Q_OBJECT

public:
    CoreApplication(int &argc, char **argv, QString version);
    ~CoreApplication();

    bool isFirst;

    bool event(QEvent *event);

    static bool versionGreater(QString firstV, QString secondV);
    static bool versionGreater(QString version);

    void checkVersion();

    static QString getAktuelleVersion();
    static QString loadVersion();

    static QUrl getUrlDownload();

public slots:
    static void closeAllWindows();

protected:
    static QString aktuelleVersion;

    static QUrl urlVersion;
    static QUrl urlDownload;
};

#endif // COREAPPLICATION_H
