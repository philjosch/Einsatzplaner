#include "crypto.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

#include <qt-aes/qaesencryption.h>


QString Cryptography::Crypto::generateSalt()
{
    QByteArray bytes;
    for (int i=0; i < 21; ++i) {
        bytes.append(QRandomGenerator::system()->bounded(1, 255));
    }
    return bytes.toBase64();
}

QByteArray Cryptography::Crypto::hash(QString password, QString salt)
{
    return QCryptographicHash::hash((password+salt).toUtf8(), QCryptographicHash::Sha3_512);
}

Cryptography::EncryptedData Cryptography::Crypto::encrypt(QString data, QString pwd)
{

    QString key = pwd;
    QString salt = Crypto::generateSalt();
    QString iv = Crypto::generateSalt();

    QByteArray hashKey = QCryptographicHash::hash((key+salt).toUtf8(), QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash(iv.toUtf8(), QCryptographicHash::Md5);

    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray encodeText = encryption.encode(data.toUtf8(), hashKey, hashIV);

    return Cryptography::EncryptedData{encodeText.toBase64(), key, salt, iv, "QtAES"};
}

QString Cryptography::Crypto::decrypt(EncryptedData encrypted)
{
    if (encrypted.typ != "QtAES")
        return "";

    QByteArray hashKey = QCryptographicHash::hash((encrypted.key+encrypted.salt).toUtf8(), QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash((encrypted.iv).toUtf8(), QCryptographicHash::Md5);

    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray decodeText = encryption.decode(QByteArray::fromBase64(encrypted.data.toUtf8()), hashKey, hashIV);

    QString decodedString = QString(encryption.removePadding(decodeText));

    return decodedString;
}
