#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QImage>

#include <Result.h>
#include <ImageView.h>

class Result : public ZXing::Result
{
public:
    explicit Result(ZXing::Result&& r)
        : ZXing::Result(std::move(r))
    {
    }

    inline QString text() const
    {
        return QString::fromStdString(ZXing::Result::text());
    }
};

inline ZXing::ImageFormat imgFmtFromQImg(const QImage &img)
{
    switch (img.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        return ZXing::ImageFormat::BGRX;
#else
        return ZXing::ImageFormat::XRGB;
#endif
    case QImage::Format_RGB888:
        return ZXing::ImageFormat::RGB;
    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
        return ZXing::ImageFormat::RGBX;
    case QImage::Format_Grayscale8:
        return ZXing::ImageFormat::Lum;
    default:
        return ZXing::ImageFormat::None;
    }
}

#endif // UTILITY_H
