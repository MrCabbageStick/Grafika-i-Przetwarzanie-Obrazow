#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "hsldialog.h"

struct HslSliderStates{
    int h, s, l; // Values from -max to max
};

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

    int d_brightness = 0;
    int d_contrast = 0;
    int d_gamma = 0;

    HslDialog *hsl_dialog = nullptr;
    HslSliderStates current_hsl = {0, 0, 0};

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void apply_changes();

    void apply_hsl();

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
