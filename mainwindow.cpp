#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QUrl>
#include <QUrlQuery>

#include <ReadBarcode.h>

#include "capturepreview.h"

#include "base32.h"
#include "otpgenerator.h"

#include "utility.h"

// https://github.com/cozmo/jsQR/tree/master
// https://github.com/scytheStudio/SCodes/blob/master/src/SBarcodeDecoder.cpp
// https://github.com/zxing-cpp/zxing-cpp/blob/master/core/src/ImageView.h
static void test()
{
    Base32 base32;

    QByteArray secret = "abcdefg";
    // 인코드-디코드 테스트
    QString enc = base32.encode(secret);
    QByteArray dec = base32.decode(enc);

    qDebug().noquote().nospace() << "base32: " << enc;
    qDebug().noquote().nospace() << "base64: " << secret.toBase64();
    qDebug().noquote().nospace() << "heximal: " << dec.toHex();

    HotpGenerator gen1(dec, QCryptographicHash::Sha1, 0, 6);
    qDebug().noquote().nospace() << "HOTP: " << gen1.generate();

    TotpGenerator gen2(dec, QCryptographicHash::Sha1, 30, 6);
    qDebug().noquote().nospace() << "TOTP: " << gen2.generate();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(Q_NULLPTR)
{
    ui->setupUi(this);

    connect(ui->buttonOpen, &QPushButton::clicked, this, &MainWindow::onOpenImageClicked);
    connect(ui->buttonSave, &QPushButton::clicked, this, &MainWindow::onSaveImageClicked);
    connect(ui->buttonCapture, &QPushButton::clicked, this, &MainWindow::onCaptureWindowClicked);
    connect(ui->buttonGenerate, &QPushButton::clicked, this, &MainWindow::onGenerateOtp);

    test();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenImageClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");

    if (filename.isEmpty() == false && QFile::exists(filename))
    {
        QImage image(filename);

        processImage(image);
    }
}

void MainWindow::onSaveImageClicked()
{
    if (pixmap.isNull())
    {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save Image", "", "Png Image (*.png)");

    if (filename.isEmpty() == false)
    {
        pixmap.save(filename);
    }
}

void MainWindow::onCaptureWindowClicked()
{
    CapturePreview preview;
    preview.showNormal();

    showMinimized();

    int ret = preview.exec();
    if (ret == 1)
    {
        processImage(preview.image());
    }

    showNormal();
}

void MainWindow::onGenerateOtp()
{
    // https://gitlab.com/dominicp/otpauth-uri-parser#readme
    QUrl url(uri);
    QUrlQuery query(url);
    qDebug().noquote().nospace() << "scheme: " << url.scheme();
    qDebug().noquote().nospace() << "path: " << url.path().mid(1);
    qDebug().noquote().nospace() << "secret: " << query.queryItemValue("secret");
    qDebug().noquote().nospace() << "issuer: " << query.queryItemValue("issuer");

    //QString name = url.path().mid(1);
    QString secret = query.queryItemValue("secret");
    //QString issuer = query.queryItemValue("issuer");
    QString digits = query.queryItemValue("digits");
    QString period = query.queryItemValue("period");

    int intDigits = digits.toInt();
    int intPeriod = period.toInt();

    intDigits = intDigits ? intDigits : 6;
    intPeriod = intPeriod ? intPeriod : 30;

    Base32 base32;
    QByteArray dec = base32.decode(secret);
    TotpGenerator gen(dec, QCryptographicHash::Sha1, intPeriod, intDigits);

    ui->lineEditSecret->setText(secret);
    ui->lineEditOtp->setText(QString("%1").arg(gen.generate(), intDigits, 10, QChar('0')));
    ui->statusbar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void MainWindow::processImage(const QImage &image)
{
    QPixmap preview;
    QImage source;

    qDebug().noquote().nospace() << "Image: " << image.width() << "x" << image.height();
    if (image.format() == QImage::Format_Mono)
    {
        source = image.convertToFormat(QImage::Format_RGBX8888);
    }
    else
    {
        source = image;
    }

    delete scene;
    uri = "";
    scene = Q_NULLPTR;
    pixmap = QPixmap();

    ZXing::ReaderOptions readerOptions = ZXing::ReaderOptions()
                                             .setFormats(ZXing::BarcodeFormat::QRCode)
                                             .setTryHarder(true).setTryRotate(true).setIsPure(false).setBinarizer(ZXing::Binarizer::LocalAverage);
    ZXing::ImageFormat imageFormat = imgFmtFromQImg(source);

    pixmap = QPixmap::fromImage(source);
    preview = pixmap;

    Result result = Result(ZXing::ReadBarcode({ source.bits(), source.width(), source.height(), imageFormat }, readerOptions));
    if (result.isValid())
    {
        ZXing::PointI tl = result.position().topLeft();
        ZXing::PointI tr = result.position().topRight();
        ZXing::PointI br = result.position().bottomRight();
        ZXing::PointI bl = result.position().bottomLeft();

        qDebug().noquote().nospace() << result.text();
        qDebug().noquote().nospace() << "Points:";
        qDebug().noquote().nospace() << " - " << tl.x << "x" << tl.y;
        qDebug().noquote().nospace() << " - " << tr.x << "x" << tr.y;
        qDebug().noquote().nospace() << " - " << br.x << "x" << br.y;
        qDebug().noquote().nospace() << " - " << bl.x << "x" << bl.y;

        QPainter p(&preview);
        p.setPen(QPen(Qt::red, 1));
        p.drawLine(QPoint(tl.x, tl.y), QPoint(tr.x, tr.y));
        p.drawLine(QPoint(tr.x, tr.y), QPoint(br.x, br.y));
        p.drawLine(QPoint(br.x, br.y), QPoint(bl.x, bl.y));
        p.drawLine(QPoint(bl.x, bl.y), QPoint(tl.x, tl.y));
        p.end();

        uri = result.text();

        onGenerateOtp();
    }

    scene = new QGraphicsScene(ui->graphicsView);
    scene->addPixmap(preview);

    ui->graphicsView->setScene(scene);
    ui->lineEditUrl->setText(uri);
}
