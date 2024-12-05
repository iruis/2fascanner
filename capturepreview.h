#ifndef CAPTUREPREVIEW_H
#define CAPTUREPREVIEW_H

#include <QDialog>
#include <QPixmap>
#include <QImage>

namespace Ui {
class CapturePreview;
}

class QGraphicsScene;
class QGraphicsRectItem;
class CapturePreview : public QDialog
{
    Q_OBJECT

public:
    explicit CapturePreview(QWidget *parent = nullptr);
    ~CapturePreview();

    QImage image();

private slots:
    void captureScreen();
    void cropPixmap();
    void cropClear();
    void submit();

private:
    Ui::CapturePreview *ui;
    QGraphicsScene *scene;
    QGraphicsRectItem *rectItem;
    QGraphicsRectItem *rectItem2;
    QPixmap pixmap;

    bool busy;
};

#endif // CAPTUREPREVIEW_H
