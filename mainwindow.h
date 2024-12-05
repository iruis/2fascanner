#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

class QImage;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenImageClicked();
    void onSaveImageClicked();
    void onCaptureWindowClicked();
    void onGenerateOtp();

private:
    void processImage(const QImage &image);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QPixmap pixmap;
    QString uri;
};

#endif // MAINWINDOW_H
