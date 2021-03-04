#include "crypto.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

#include <qt-aes/qaesencryption.h>

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

Crypto::EncryptedData Crypto::encrypt(QString data, QString pwd)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

    QString key = pwd;
    QString salt = Crypto::generateSalt();
    QString iv = Crypto::generateSalt();

    QByteArray hashKey = QCryptographicHash::hash((key+salt).toUtf8(), QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash(iv.toUtf8(), QCryptographicHash::Md5);

    QByteArray encodeText = encryption.encode(data.toUtf8(), hashKey, hashIV);

    return Crypto::EncryptedData{encodeText.toBase64(), key, salt, iv};
}

QString Crypto::decrypt(Crypto::EncryptedData encrypted)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

    QByteArray hashKey = QCryptographicHash::hash((encrypted.key+encrypted.salt).toUtf8(), QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash((encrypted.iv).toUtf8(), QCryptographicHash::Md5);

    QByteArray decodeText = encryption.decode(QByteArray::fromBase64(encrypted.data.toUtf8()), hashKey, hashIV);

    QString decodedString = QString(encryption.removePadding(decodeText));

    return decodedString;
}