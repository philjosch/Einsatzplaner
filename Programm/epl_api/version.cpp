#include "networking.h"
#include "version.h"

#include <QCoreApplication>

Version Version::VERSION = Version();
bool Version::IS_DEPLOY_VERSION = false;
bool Version::IS_DEBUG_BUILD = false;
QString Version::BUILD_NUMBER = "???????";

QString Version::URL_VERSION = "http://epl.philipp-schepper.de/version.txt";
QString Version::URL_NOTES = "http://epl.philipp-schepper.de/version/v%1-%2/notes-v%1-%2-%3.txt";

Version::Version(QString vers)
{
    if (! vers.contains(".")) {
        major = -1;
        minor = -1;
        patch = -1;
        return;
    }
    QStringList versList = vers.split(".");
    if (versList.length() < 2){
        major = -1;
        minor = -1;
        patch = -1;
        return;
    }
    major = versList.at(0).toInt();
    minor = versList.at(1).toInt();
    if (versList.length() >= 3) // Dritte Stelle nur nehmen, wenn sie vorhanden ist
        patch = versList.at(2).toInt();
    else
        patch = -1;
}
Version::Version(int maj, int min)
{
    major = maj;
    minor = min;
    patch = -1;
}
Version::Version(int maj, int min, int pat)
{
    major = maj;
    minor = min;
    patch = pat;
}

QString Version::toString()
{
    return QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
}
QString Version::toStringShort()
{
    return QString("%1.%2").arg(major).arg(minor);
}

int Version::getMajor() const
{
    return major;
}
int Version::getMinor() const
{
    return minor;
}
int Version::getPatch() const
{
    return patch;
}


bool operator==(const Version &lhs, const Version rhs)
{
    if ((lhs.major == rhs.major) && (lhs.minor == rhs.minor) && (lhs.patch == rhs.patch)) {
        return true;
    }
    return false;
}
bool operator>(const Version &lhs, const Version rhs)
{
    if ((lhs.major > rhs.major) || (rhs.major == -1)) {
        return true;
    }
    if ((lhs.major == rhs.major) && ((lhs.minor > rhs.minor) || (rhs.minor == -1))) {
        return true;
    }
    if ((lhs.major == rhs.major) && (lhs.minor == rhs.minor) && ((lhs.patch > rhs.patch) || (lhs.patch != -1 && rhs.patch == -1))) {
        return true;
    }
    return false;
}
bool operator<(const Version &lhs, const Version rhs)
{
    return rhs > lhs;
}
bool operator>=(const Version &lhs, const Version rhs)
{
    return !(lhs < rhs);
}
bool operator<=(const Version &lhs, const Version rhs)
{
    return !(lhs > rhs);
}

void Version::setProgrammVersion(Version akt, bool deploy, bool debug, QString build)
{
    VERSION = akt;
    IS_DEPLOY_VERSION = deploy;
    IS_DEBUG_BUILD = debug;
    BUILD_NUMBER = build;
    QCoreApplication::setApplicationVersion(getKompletteVersionsnummer());
}
QString Version::getKompletteVersionsnummer()
{
    QString s = VERSION.toString()+" (%1: %2)";
    if (IS_DEPLOY_VERSION) {
        s = s.arg("rev");
    } else {
        s = s.arg("dev");
    }
    s = s.arg(BUILD_NUMBER);
    if (IS_DEBUG_BUILD) {
        s = s + " (deb)";
    }
    return s;
}
Version Version::getProgrammVersion()
{
    return VERSION;
}


bool Version::isDeveloperVersion()
{
    return !IS_DEPLOY_VERSION;
}
bool Version::isDebuggingVersion()
{
    return IS_DEBUG_BUILD;
}


bool Version::isSupportedVersion(Version test) {
    if (test == Version{-1,-1,-1}) return false;
    if (test > VERSION) return false;
    return true;
}


bool Version::isUpdateVerfuegbar()
{
    Version v = ladeNeusteVersion();
    return (v>Version::VERSION) || ((v == Version::VERSION) && !Version::IS_DEPLOY_VERSION);
}
Version Version::ladeNeusteVersion()
{
    return Version(Networking::ladeDatenVonURL(URL_VERSION));
}
QString Version::loadNotes(Version v)
{
    return Networking::ladeDatenVonURL(URL_NOTES.arg(v.getMajor()).arg(v.getMinor()).arg(v.getPatch()));
}
