#ifndef OTPGENERATOR_H
#define OTPGENERATOR_H

#include <QByteArray>
#include <QCryptographicHash>

class OtpGenerator
{
private:
    QByteArray secret;
    QCryptographicHash::Algorithm algorithm;
    int digits;

public:
    OtpGenerator(const QByteArray secret, QCryptographicHash::Algorithm algorithm, int digits = 6);

    int generateForCounter(quint64 counter);

protected:
    virtual int generate() = 0;
};

class HotpGenerator : public OtpGenerator
{
private:
    quint64 counter;

public:
    HotpGenerator(const QByteArray secret, QCryptographicHash::Algorithm algorithm, quint64 counter = 0, int digits = 6);

    int generate() override;
};


class TotpGenerator : public OtpGenerator
{
private:
    int period;

public:
    TotpGenerator(const QByteArray secret, QCryptographicHash::Algorithm algorithm, int period = 30, int digits = 6);

    int generate() override;
};

#endif // OTPGENERATOR_H
