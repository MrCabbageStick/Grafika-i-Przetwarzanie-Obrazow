#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "hsldialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QImage original_image;

    int d_brightness = 0;
    int d_contrast = 0;
    int d_gamma = 0;

    HslDialog *hsl_dialog = nullptr;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void apply_changes();

public slots:
    void load_image_pushed();
    void brightness_slider_changed(int value);
    void contrast_slider_changed(int value);
    void gamma_slider_changed(int value);

    /// values from 0 to 359, treat as degrees
    void hueSliderChanged(int value);
    /// values from 0 to 99, treat as percents
    void satSliderChanged(int value);
    /// values from 0 to 99, treat as percents
    void lightSliderChanged(int value);

private slots:
    void on_actionHSL_triggered();
};
#endif // MAINWINDOW_H
