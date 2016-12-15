#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include <QApplication>

class CoreApplication : public QApplication
{
    Q_OBJECT

public:
    CoreApplication(int &argc, char **argv, QString version);
    ~CoreApplication();

    struct Version {
        int major;
        int minor;
        int patch;
    };

    bool isFirst;

    bool event(QEvent *event);

    static bool versionGreater(QString firstV, QString secondV);
    static bool versionGreater(QString version);

    static bool versionGreaterMajor(QString firstV, QString secondV);
    static bool versionGreaterMajor(QString version);

    void checkVersion();

    static QString getAktuelleVersion();
    static QString getAktuelleVersionKurz();
    static QString loadVersion();
    static QString loadNotes(Version v);

    static QUrl getUrlDownload();

    static Version versionParser(QString version);

public slots:
    static void closeAllWindows();

protected:
    static QString aktuelleVersion;
    static QString aktuelleVersionKurz;

    static QUrl urlVersion;
    static QUrl urlDownload;
    static QString urlNotes;
};

#endif // COREAPPLICATION_H
