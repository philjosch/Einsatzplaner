#ifndef QFAHRTAG_H
#define QFAHRTAG_H

#include <QObject>
#include <QDate>

class QFahrtag
{

public:
    QFahrtag();
    ~QFahrtag();
private:
    QDate Datum;
    int Art;
    bool Wichtig;
    QString Anlass;
    QString Wagen;
    QString Tf;
    QString Zf;
    QString Zub;
    QString Service;
    QTime ZeitTf;
    QTime ZeitZ;
    QString Reservierungen;
    QString Bemerkungen;

private slots:
    void weiterleiten();
public slots:
    // GETTER
    QDate getDatum();
    int getArt();
    bool getWichtig();
    QString getAnlass();
    // Get Wagenreihung
    QString getTextTf();
    QString getTextZf();
    QString getTextZub();
    QString getTextService();
    QTime getTimeTf();
    QTime getTimeZ();
    QString getReservierungen();
    QString getBemerkungen();



    // SETTER
    void setDatum(QDate datum);
    void setArt(int art);
    void setWichtig(bool wichtig);
    void setAnlass(QString anlass);
    // Set Wagenreihung
    void setTextTf(QString text);
    void setTextZf(QString text);
    void setTextZub(QString text);
    void setTextService(QString text);
    void setTimeTf(QTime zeit);
    void setTimeZ(QTime zeit);
    void setTextReservierungen(QString text);
    void setTextBemerkungen(QString text);

};

#endif // QFAHRTAG_H
