#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QImage>

#include "algorithm.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->load_image_btn, SIGNAL(clicked(bool)), this, SLOT(load_image_pushed()));
    connect(ui->gamma_slider, SIGNAL(valueChanged(int)), SLOT(gamma_slider_changed(int)));
    connect(ui->contrast_slider, SIGNAL(valueChanged(int)), SLOT(contrast_slider_changed(int)));
    connect(ui->brightness_slider, SIGNAL(valueChanged(int)), SLOT(brightness_slider_changed(int)));

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

    QPixmap pixmap = QPixmap::fromImage(original_image);

    ui->orignal_image->setPixmap(pixmap);
    ui->edited_image->setPixmap(pixmap);

    edited_image = QImage(original_image);

    update_histogram();
}

void MainWindow::apply_changes(){
    if(original_image.isNull()){
        return;
    }

    edited_image = QImage(original_image);

    run_algorithm(original_image, edited_image, AlgorithmArgs{ d_brightness, d_contrast, d_gamma });

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
    ui->histogram_label->setPixmap(QPixmap::fromImage(histogram_image));

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


