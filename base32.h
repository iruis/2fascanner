#ifndef BASE32_H
#define BASE32_H

#include <QByteArray>
#include <QString>

// https://gist.github.com/erdomke/9335c394c5cc65404c4cf9aceab04143
class Base32
{
private:
    enum
    {
        MASK = 31,
        SHIFT = 5,
    };

public:
    QByteArray decode(const QString &base32) const;
    QString encode(const QByteArray &data, qsizetype offset = 0, bool padOutput = true) const;
};

#endif // BASE32_H
