#include "crypto.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

QString Crypto::generateSalt()
{
    QByteArray bytes;
    for (int i=0; i < 21; ++i) {
        bytes.append(QRandomGenerator::system()->bounded(1, 255));
    }
    return bytes.toBase64();
}

QByteArray Crypto::hash(QString password, QString salt)
{
    return QCryptographicHash::hash((password+salt).toUtf8(), QCryptographicHash::Sha3_512);
}
