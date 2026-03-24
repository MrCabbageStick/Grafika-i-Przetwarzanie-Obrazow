#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QImage original_image;
    QImage edited_image;

    QImage histogram_image;

    int d_brightness = 0;
    int d_contrast = 0;
    int d_gamma = 0;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void apply_changes();
    void update_histogram();

public slots:
    void load_image_pushed();
    void brightness_slider_changed(int value);
    void contrast_slider_changed(int value);
    void gamma_slider_changed(int value);

};
#endif // MAINWINDOW_H
