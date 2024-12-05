#include "otpgenerator.h"

#include <QtEndian>
#include <QDateTime>
#include <QMessageAuthenticationCode>

constexpr int kPinModTable[] = {
    0,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
};

OtpGenerator::OtpGenerator(const QByteArray secret, QCryptographicHash::Algorithm algorithm, int digits)
    : secret(secret)
    , algorithm(algorithm)
    , digits(digits)
{
    switch (algorithm)
    {
    default: throw "bad algorithm";
    case QCryptographicHash::Sha1:
    case QCryptographicHash::Sha256:
    case QCryptographicHash::Sha512:
    case QCryptographicHash::Md5:
        break;
    }
}

int OtpGenerator::generateForCounter(quint64 counter)
{
    QMessageAuthenticationCode hmac(algorithm, secret);
    int hashLength = QCryptographicHash::hashLength(algorithm);

    counter = qToBigEndian(counter);

    hmac.addData(reinterpret_cast<char*>(&counter), sizeof(counter));

    QByteArray hash = hmac.result();

    int offset = hash[hashLength - 1] & 0x0f;
    int truncated = 0; //qFromBigEndian(*reinterpret_cast<uint32_t*>(&hash[offset])) & 0x7fffffff;

    truncated |= (hash[offset + 0] & 0x7f) << 24;
    truncated |= (hash[offset + 1] & 0xff) << 16;
    truncated |= (hash[offset + 2] & 0xff) <<  8;
    truncated |= (hash[offset + 3] & 0xff) <<  0;

    int pinValue = truncated % kPinModTable[digits];

    return pinValue;
}

HotpGenerator::HotpGenerator(const QByteArray secret, QCryptographicHash::Algorithm algorithm, quint64 counter, int digits)
    : OtpGenerator(secret, algorithm, digits)
    , counter(counter)
{
}

int HotpGenerator::generate()
{
    return generateForCounter(counter);
}

TotpGenerator::TotpGenerator(const QByteArray secret, QCryptographicHash::Algorithm algorithm, int period, int digits)
    : OtpGenerator(secret, algorithm, digits)
    , period(period)
{
}

int TotpGenerator::generate()
{
    QDateTime date = QDateTime::currentDateTimeUtc();
    quint64 counter = date.toSecsSinceEpoch() / period;

    return generateForCounter(counter);
}
