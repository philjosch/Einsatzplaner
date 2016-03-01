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
    QString wagen;
    QList<QString> *Tf;
    QList<QString> *Zf;
    QList<QString> *Zub;
    QList<QString> *Service;
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
    QString getWagenreihung();
    QTime getTimeTf();
    QTime getTimeZ();

    QList<QString>* getTf();
    QList<QString>* getZf();
    QList<QString>* getZub();
    QList<QString>* getService();
    QString getReservierungen();
    QString getBemerkungen();



    // SETTER
    void setDatum(QDate datum);
    void setArt(int art);
    void setWichtig(bool wichtig);
    void setAnlass(QString anlass);
    void setWagenreihung(QString wagenreihung);
    void setTimeTf(QTime zeit);
    void setTimeZ(QTime zeit);

    void setTf(QList<QString> *text);
    void setZf(QList<QString> *text);
    void setZub(QList<QString> *text);
    void setService(QList<QString> *text);
    void setTextReservierungen(QString text);
    void setTextBemerkungen(QString text);

};

#endif // QFAHRTAG_H
