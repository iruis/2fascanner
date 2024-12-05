#include "capturepreview.h"
#include "ui_capturepreview.h"

#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGuiApplication>
#include <QScreen>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

CapturePreview::CapturePreview(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CapturePreview)
    , rectItem(Q_NULLPTR)
    , rectItem2(Q_NULLPTR)
    , busy(false)
{
    scene = new QGraphicsScene(this);

    ui->setupUi(this);
    ui->graphicsView->setScene(scene);

    captureScreen();

    connect(ui->slX, &QSlider::valueChanged, this, &CapturePreview::cropPixmap);
    connect(ui->slY, &QSlider::valueChanged, this, &CapturePreview::cropPixmap);
    connect(ui->slW, &QSlider::valueChanged, this, &CapturePreview::cropPixmap);
    connect(ui->slH, &QSlider::valueChanged, this, &CapturePreview::cropPixmap);
    connect(ui->buttonRefresh, &QPushButton::clicked, this, &CapturePreview::captureScreen);
    connect(ui->buttonClear, &QPushButton::clicked, this, &CapturePreview::cropClear);
    connect(ui->buttonSubmit, &QPushButton::clicked, this, &CapturePreview::submit);
}

CapturePreview::~CapturePreview()
{
    delete ui;
}

QImage CapturePreview::image()
{
    if (rectItem)
    {
        return pixmap.copy(rectItem->rect().toRect()).toImage();
    }
    return pixmap.toImage();
}

void CapturePreview::captureScreen()
{
    int index = ui->comboScreen->currentIndex();
    if (index < 0)
    {
        index = 0;
    }
    ui->comboScreen->clear();

    QList<QScreen *> screens = QGuiApplication::screens();
    if (index >= screens.size())
    {
        return;
    }
    for (int i = 0; i < screens.size(); i++)
    {
        QScreen *screen = screens.at(i);
        QRect rect = screen->geometry();

        ui->comboScreen->addItem(QString("%1 (%2,%3, %4x%5").arg(screen->name().trimmed()).arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height()));

        if (i == index)
        {
            ui->comboScreen->setCurrentIndex(index);
        }
    }

    QRect rect = screens.at(index)->geometry();

    HDC hdc = GetDC(NULL);
    HDC hDest = CreateCompatibleDC(hdc);

    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HBITMAP hbDesktop = CreateCompatibleBitmap(hdc, width, height);

    SelectObject(hDest, hbDesktop);

    BitBlt(hDest, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    busy = true;
    pixmap = QPixmap::fromImage(QImage::fromHBITMAP(hbDesktop)).copy(rect);

    delete rectItem;
    delete rectItem2;

    scene->clear();
    scene->addPixmap(pixmap);
    rectItem = Q_NULLPTR;
    rectItem2 = Q_NULLPTR;

    ui->slX->setMaximum(rect.width() - 1);
    ui->slY->setMaximum(rect.height() - 1);
    ui->slW->setMaximum(rect.width());
    ui->slH->setMaximum(rect.height());

    busy = false;

    ReleaseDC(NULL, hdc);
    DeleteDC(hDest);
}

void CapturePreview::cropPixmap()
{
    if (busy)
    {
        return;
    }

    int x = ui->slX->value();
    int y = ui->slY->value();
    int w = ui->slW->value();
    int h = ui->slH->value();

    int screenW = ui->slW->maximum();
    int screenH = ui->slH->maximum();

    if (pixmap.isNull())
    {
        return;
    }
    if (x + w > screenW || y + h > screenH)
    {
        return;
    }

    QRect rect(x, y, w, h);
    if (rectItem)
    {
        rectItem->setRect(rect);
        rectItem2->setRect(rect);
    }
    else
    {
        rectItem2 = scene->addRect(rect, QPen(Qt::black, 3));
        rectItem = scene->addRect(rect, QPen(Qt::white, 1), QBrush(QColor(255, 255, 255, 32)));
    }
}

void CapturePreview::cropClear()
{
    delete rectItem;
    delete rectItem2;

    rectItem = Q_NULLPTR;
    rectItem2 = Q_NULLPTR;
}

void CapturePreview::submit()
{
    setResult(1);

    accept();
}
