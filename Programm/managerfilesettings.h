#ifndef MANAGERFILESETTINGS_H
#define MANAGERFILESETTINGS_H

#include <QJsonObject>
#include <QString>

class ManagerFileSettings
{
public:
    ManagerFileSettings();

    void fromJson(QJsonObject obj);
    QJsonObject toJson();

    bool getEnabled() const;
    void setEnabled(bool value);

    bool getAutom() const;
    void setAutom(bool value);

    QString getServer() const;
    void setServer(const QString &value);

    QString getPath() const;
    void setPath(const QString &value);

    QString getFullServer() const;

    QString getId() const;
    void setId(const QString &value);

    QString getStartdate() const;
    void setStartdate(const QString &value);

    QString getEnddate() const;
    void setEnddate(const QString &value);

    bool getActivities() const;
    void setActivities(bool value);

protected:
    bool enabled = false;
    bool autom = true;
    QString server = "";
    QString path = "";
    QString id = "";
    QString startdate = "tdy";
    QString enddate = "all";
    bool activities = true;
};

#endif // MANAGERFILESETTINGS_H
