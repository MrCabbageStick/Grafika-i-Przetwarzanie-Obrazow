#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "hsldialog.h"
#include "labdialog.h"

struct HslSliderStates{
    int h, s, l; // Values from -max to max
};

struct LabSliderStates{
    int l, a, b;
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

    LabDialog *lab_dialog = nullptr;
    LabSliderStates current_lab = {0, 0, 0};

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void apply_changes();

    void apply_hsl();
    void apply_lab();

public slots:
    void load_image_pushed();
    void brightness_slider_changed(int value);
    void contrast_slider_changed(int value);
    void gamma_slider_changed(int value);

    void hueSliderChanged(int value);
    void satSliderChanged(int value);
    void lightSliderChanged(int value);

    void lSliderChanged(int value);
    void aSliderChanged(int value);
    void bSliderChanged(int value);

private slots:
    void on_actionHSL_triggered();
};
#endif // MAINWINDOW_H
