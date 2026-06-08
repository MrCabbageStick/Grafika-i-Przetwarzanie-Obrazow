#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "hsldialog.h"
#include "labdialog.h"
#include "transformdialog.h"

#include "histogramdialog.h"

struct HslSliderStates{
    int h, s, l; // Values from -max to max
};

struct LabSliderStates{
    int l, a, b;
};

struct TransformState{
    int tx, ty;
    float rot;
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
    QImage *editing_copy = nullptr;

    QImage histogram_image;

    int d_brightness = 0;
    int d_contrast = 0;
    int d_gamma = 0;

    TransformState transform_state = {0, 0, 0};

    HslDialog *hsl_dialog = nullptr;
    HslSliderStates current_hsl = {0, 0, 0};

    LabDialog *lab_dialog = nullptr;
    LabSliderStates current_lab = {0, 0, 0};

    HistogramDialog *histogramDialog = nullptr;

    TransformDialog *transformDialog = nullptr;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void apply_changes();

    void apply_hsl();
    void apply_lab();

    void update_histogram();

    void transform(int tx, int ty, float rot);
    void commitTransform();
    void cancelTransform();
    void updateTransformSlider();

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

    void rotationSliderChanged(int value);
    void xTranslationChanged(int value);
    void yTranslationChanged(int value);

private slots:
    void on_actionHSL_triggered();
};
#endif // MAINWINDOW_H
