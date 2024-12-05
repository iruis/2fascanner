#include "base32.h"

#include <QVector>

static int charToValue(char c)
{
    // uppercase letters
    if (c < 91 && c > 64)
    {
        return c - 65;
    }
    // numbers 2~7
    if (c < 56 && c > 49)
    {
        return c - 24;
    }
    // lowercase letters
    if (c < 123 && c > 96)
    {
        return c - 97;
    }
    return -1;
}

QByteArray Base32::decode(const QString &base32) const
{
    int idx = base32.indexOf('=');
    int len = base32.length();
    if (idx >= 0)
    {
        len = idx;
    }
    if (len == 0)
    {
        return QByteArray();
    }

    qsizetype outLength = len * SHIFT / 8;
    QByteArray data(outLength, char(0));

    int next = 0;
    int buffer = 0;
    int bitsLeft = 0;

    for (qsizetype i = 0; i < len; i++)
    {
        QChar c = base32.at(i);
        int charValue = charToValue(c.toLatin1());
        if (charValue < 0)
        {
            throw "Character is not a Base32 character.";
        }

        buffer <<= SHIFT;
        buffer |= charValue & MASK;
        bitsLeft += SHIFT;
        if (bitsLeft >= 8)
        {
            data[next++] = buffer >> (bitsLeft - 8);
            bitsLeft -= 8;
        }
    }
    return data;
}

QString Base32::encode(const QByteArray &data, qsizetype offset, bool padOutput) const
{
    if (data.isEmpty())
    {
        throw "data is empty";
    }
    char digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    qsizetype len = qsizetype((data.length() * 8 + SHIFT - 1) / SHIFT);
    QVector<QChar> base32;

    base32.reserve(len);

    qsizetype last = offset + data.length();
    int buffer = data[offset++];
    int bitsLeft = 8;
    while (bitsLeft > 0 || offset < last)
    {
        if (bitsLeft < SHIFT)
        {
            if (offset < last)
            {
                buffer <<= 8;
                buffer |= data[offset++] & 0xff;
                bitsLeft += 8;
            }
            else
            {
                int pad = SHIFT - bitsLeft;
                buffer <<= pad;
                bitsLeft += pad;
            }
        }
        int index = MASK & (buffer >> (bitsLeft - SHIFT));
        bitsLeft -= SHIFT;
        base32.append(digits[index]);
    }

    if (padOutput)
    {
        int padding = (8 - (base32.length() % 8)) % 8;
        if (padding > 0)
        {
            base32.append(QList<QChar>(padding, '='));
        }
    }
    return QString(base32.data(), base32.length());
}
