#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QImage>

#include "algorithm.h"
#include "ui_histogramdialog.h"
#include "ui_hsldialog.h"
#include "color_spaces.h"
#include "tools.h"
#include "ui_labdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    histogramDialog = new HistogramDialog(this);

    hsl_dialog = new HslDialog(this);
    lab_dialog = new LabDialog(this);

    connect(ui->load_image_btn, SIGNAL(clicked(bool)), this, SLOT(load_image_pushed()));
    connect(ui->gamma_slider, SIGNAL(valueChanged(int)), SLOT(gamma_slider_changed(int)));
    connect(ui->contrast_slider, SIGNAL(valueChanged(int)), SLOT(contrast_slider_changed(int)));
    connect(ui->brightness_slider, SIGNAL(valueChanged(int)), SLOT(brightness_slider_changed(int)));

    connect(ui->actionHSL, &QAction::triggered, this, [this]() {
        hsl_dialog->show();
        hsl_dialog->raise();      // bring to front if already open
        hsl_dialog->activateWindow();
    });

    connect(ui->actionLAB, &QAction::triggered, this, [this]() {
        lab_dialog->show();
        lab_dialog->raise();      // bring to front if already open
        lab_dialog->activateWindow();
    });

    connect(ui->actionHistogram, &QAction::triggered, this, [this](){
        histogramDialog->show();
        histogramDialog->raise();      // bring to front if already open
        histogramDialog->activateWindow();
    });

    connect(hsl_dialog->ui->h_slider, &QSlider::valueChanged,
            this, &MainWindow::hueSliderChanged);
    connect(hsl_dialog->ui->s_slider, &QSlider::valueChanged,
            this, &MainWindow::satSliderChanged);
    connect(hsl_dialog->ui->l_slider, &QSlider::valueChanged,
            this, &MainWindow::lightSliderChanged);

    connect(lab_dialog->ui->l_slider, &QSlider::valueChanged,
            this, &MainWindow::lSliderChanged);
    connect(lab_dialog->ui->a_slider, &QSlider::valueChanged,
            this, &MainWindow::aSliderChanged);
    connect(lab_dialog->ui->b_slider, &QSlider::valueChanged,
            this, &MainWindow::bSliderChanged);

    BGRA color = {123, 255, 9, 255};
    Hsl color_hsl = rgb2hsl(color);
    BGRA color_back = hsl2rgb(color_hsl);
    qDebug()
        << "Rgb: " << color.r << ", " << color.g << ", " << color.b
        << " | Hsl: " << color_hsl.h << ", " << color_hsl.s << ", " << color_hsl.l
        << " | Rgb: " << color_back.r << ", " << color_back.g << ", " << color_back.b
        << "\n";

    XYZ xyz = rgb2xyz(color);
    BGRA color_from_xyz = xyz2rgb(xyz);
    qDebug()
        << "Rgb: " << color.r << ", " << color.g << ", " << color.b
        << " | Xyz: " << xyz.x << ", " << xyz.y << ", " << xyz.z
        << " | Rgb: " << color_from_xyz.r << ", " << color_from_xyz.g << ", " << color_from_xyz.b
        << "\n";

    LAB lab = rgb2lab(color);
    BGRA color_from_lab = lab2rgb(lab);
    qDebug()
        << "Rgb: " << color.r << ", " << color.g << ", " << color.b
        << " | Lab: " << lab.l << ", " << lab.a << ", " << lab.b
        << " | Rgb: " << color_from_lab.r << ", " << color_from_lab.g << ", " << color_from_lab.b
        << "\n";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_image_pushed(){
    qDebug() << "Button pushed :)";

    QString fname = QFileDialog::getOpenFileName();

    if(fname.isEmpty()){
        return;
    }

    qDebug() << fname;

    if(!original_image.load(fname)){
        qDebug() << "Unable to load image " << fname;
        return;
    }

    qDebug() << "Loaded image: " << original_image;
    edited_image = QImage(original_image);

    QPixmap pixmap = QPixmap::fromImage(original_image);

    ui->orignal_image->setPixmap(pixmap);

    update_histogram();
}

void MainWindow::apply_changes(){
    if(original_image.isNull() || original_image.isNull()){
        return;
    }

    // QImage local_image = QImage(original_image);

    run_algorithm(original_image, edited_image, AlgorithmArgs{ d_brightness, d_contrast, d_gamma });

    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));

    update_histogram();
}

void MainWindow::brightness_slider_changed(int value){
    d_brightness = value;
    apply_changes();
}

void MainWindow::contrast_slider_changed(int value){
    d_contrast = value;
    apply_changes();
}

void MainWindow::gamma_slider_changed(int value){
    d_gamma = value;
    apply_changes();
}



void MainWindow::on_actionHSL_triggered(){}



void MainWindow::hueSliderChanged(int value){
    current_hsl.h = value;
    apply_hsl();
}

void MainWindow::satSliderChanged(int value){
    current_hsl.s = value;
    apply_hsl();
}

void MainWindow::lightSliderChanged(int value){
    current_hsl.l = value;
    apply_hsl();
}

void MainWindow::apply_hsl(){
    hsl_dialog->ui->s_slider->setValue(current_hsl.s);
    hsl_dialog->ui->l_slider->setValue(current_hsl.l);
    hsl_dialog->ui->h_slider->setValue(current_hsl.h);

    for(int y = 0; y < edited_image.height(); y++){
        auto dst_line = reinterpret_cast<QRgb*>(edited_image.scanLine(y));
        auto src_line = reinterpret_cast<const QRgb*>(original_image.constScanLine(y));

        for(int x = 0; x < edited_image.width(); x++){
            // Use helper structure to extract RGBA data from QRgb
            const BGRA* src_pixel = (const BGRA*)(src_line + x);
            BGRA* dst_pixel = (BGRA*)(dst_line + x);

            Hsl hsl = rgb2hsl(*src_pixel);

            hsl.h = (hsl.h + (double)current_hsl.h);
            if(hsl.h > 359) hsl.h -= 360; else if(hsl.h < 0) hsl.h += 360;

            hsl.s = std::clamp(hsl.s + current_hsl.s / 100.0, 0.0, 1.0);
            hsl.l = std::clamp(hsl.l + current_hsl.l / 100.0, 0.0, 1.0);

            *dst_pixel = hsl2rgb(hsl);
        }
    }

    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));

    update_histogram();
}


void MainWindow::lSliderChanged(int value){
    current_lab.l = value;
    apply_lab();
}

void MainWindow::aSliderChanged(int value){
    current_lab.a = value;
    apply_lab();
}

void MainWindow::bSliderChanged(int value){
    current_lab.b = value;
    apply_lab();
}

void MainWindow::apply_lab(){
    lab_dialog->ui->l_slider->setValue(current_lab.l);
    lab_dialog->ui->a_slider->setValue(current_lab.a);
    lab_dialog->ui->b_slider->setValue(current_lab.b);

    for(int y = 0; y < edited_image.height(); y++){
        auto dst_line = reinterpret_cast<QRgb*>(edited_image.scanLine(y));
        auto src_line = reinterpret_cast<const QRgb*>(original_image.constScanLine(y));

        for(int x = 0; x < edited_image.width(); x++){
            // Use helper structure to extract RGBA data from QRgb
            const BGRA* src_pixel = (const BGRA*)(src_line + x);
            BGRA* dst_pixel = (BGRA*)(dst_line + x);

            LAB lab = rgb2lab(*src_pixel);

            lab.l = std::clamp(lab.l + current_lab.l, 0.0f, 100.0f);
            lab.a = std::clamp(lab.a + current_lab.a, -128.0f, 127.0f);
            lab.b = std::clamp(lab.b + current_lab.b, -128.0f, 127.0f);

            *dst_pixel = lab2rgb(lab);
        }
    }

    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));

    update_histogram();
}

void MainWindow::update_histogram(){
    size_t n_pixels = edited_image.width() * edited_image.height();

    auto bits = reinterpret_cast<BGRA*>(edited_image.bits());

    float green_intesities[256] = {};
    float blue_intesities[256] = {};
    float red_intesities[256] = {};


    // Count pixels with intesity <0; 255>
    for(size_t i = 0; i < n_pixels; i++){
        auto rgba = bits[i];

        green_intesities[rgba.g]++;
        blue_intesities[rgba.b]++;
        red_intesities[rgba.r]++;
    }

    // green_intesities[0] = 0;
    // red_intesities[0] = 0;
    // blue_intesities[0] = 0;

    // green_intesities[255] = 0;
    // red_intesities[255] = 0;
    // blue_intesities[255] = 0;


    // for(size_t i = 0; i < 256; i++){
    //     green_intesities[i] = std::log1p(green_intesities[i]);
    //     red_intesities[i] = std::log1p(red_intesities[i]);
    //     blue_intesities[i] = std::log1p(blue_intesities[i]);
    // }

    float max_pixel_count = 0;

    for(size_t i = 0; i < 256; i++){
        max_pixel_count = std::max({green_intesities[i], red_intesities[i], blue_intesities[i], max_pixel_count});
    }

    // Convert pixel count to percent of image pixels
    for(size_t i = 0; i < 256; i++){
        green_intesities[i] /= max_pixel_count;
        red_intesities[i] /= max_pixel_count;
        blue_intesities[i] /= max_pixel_count;
    }

    histogram_image = QImage(256, 100, QImage::Format_ARGB32);
    histogram_image.fill(QColor::fromRgb(0, 0, 0));

    auto histogram_bits = reinterpret_cast<BGRA*>(histogram_image.bits());

    for(size_t x = 0; x < histogram_image.width(); x++){
        for(size_t y = 0; y < histogram_image.height(); y++){
            auto pixel = &histogram_bits[x + y * histogram_image.width()];

            if(green_intesities[x] * 100 > y) pixel->g = 255;
            if(red_intesities[x] * 100 > y) pixel->r = 255;
            if(blue_intesities[x] * 100 > y) pixel->b = 255;

        }
    }

    histogram_image.flip();
    histogramDialog->ui->histogram_label->setPixmap(QPixmap::fromImage(histogram_image));

}

