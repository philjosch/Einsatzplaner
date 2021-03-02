#ifndef CRYPTO_H
#define CRYPTO_H

#include <QObject>

class Crypto
{
public:
    struct EncryptedData {
        QString data;
        QString key;
        QString salt;
        QString iv;
//        QString typ = "qt-aes";
    };

    static QString generateSalt();

    static QByteArray hash(QString password, QString salt = "jbkOEGsifP8sKkXE4lUK");

    static EncryptedData encrypt(QString data, QString pwd);
    static QString decrypt(EncryptedData encrypted);
};

#endif // CRYPTO_H
