#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include <QApplication>

class CoreApplication : public QApplication
{
    Q_OBJECT

public:
    struct Version {
        int major;
        int minor;
        int patch;

        QString toString() {
            return QString::number(major)+"."+QString::number(minor)+"."+QString::number(patch);
        }
        QString toStringShort() {
            return QString::number(major)+"."+QString::number(minor);
        }
        static Version stringToVersion (QString s) {
            if (! s.contains(".")) return Version {-1, -1, -1};
            QStringList vers = s.split(".");
            if (vers.length() < 2) return Version {-1, -1, -1};
            Version v;
            v.major = vers.at(0).toInt();
            v.minor = vers.at(1).toInt();
            if (vers.length() == 3) // Dritte Stelle nur nehmen, wenn sie vorhanden ist
                v.patch = vers.at(2).toInt();
            else
                v.patch = -1;
            return v;
        }

        bool operator ==(Version second) {
            if ((major == second.major) && (minor == second.minor) && (patch == second.patch)) {
                return true;
            }
            return false;
        }
        bool operator >(Version second) {
            if ((major > second.major) || (second.major == -1)) {
                return true;
            }
            if ((major == second.major) && ((minor > second.minor) || (second.minor == -1))) {
                return true;
            }
            if ((major == second.major) && (minor == second.minor) && ((patch > second.patch) || (second.patch == -1))) {
                return true;
            }
            return false;
        }
        bool operator <(Version second) {
            return second>*this;
        }
        bool operator >=(Version second) {
            return *this == second || *this > second;
        }
        bool operator <=(Version second) {
            return *this == second || second > *this;
        }
    };

    CoreApplication(int &argc, char **argv, CoreApplication::Version version, bool devVersion);
    ~CoreApplication();

    bool getIsFirst() const;

    bool event(QEvent *event);

    void checkVersion();

    void startAutoSave(int delay); // Delay in Seconds!
    void stopAutoSave();

    static bool isDeveloperVersion();
    static Version getAktuelleVersion();
    static Version loadVersion();

    static QString loadNotes(Version v);

    static QUrl getUrlDownload();

public slots:
    static void closeAllWindows();
    static void autoSaveWindows();

protected:
    static Version aktuelleVersion;
    static bool developerMode;

    static QUrl urlVersion;
    static QUrl urlDownload;
    static QString urlNotes;

    QTimer *autoSaveTimer;
    bool isFirst;

};

#endif // COREAPPLICATION_H
