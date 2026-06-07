#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QImage>

#include "algorithm.h"
#include "ui_hsldialog.h"
#include "color_spaces.h"
#include "tools.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    hsl_dialog = new HslDialog(this);

    connect(ui->load_image_btn, SIGNAL(clicked(bool)), this, SLOT(load_image_pushed()));
    connect(ui->gamma_slider, SIGNAL(valueChanged(int)), SLOT(gamma_slider_changed(int)));
    connect(ui->contrast_slider, SIGNAL(valueChanged(int)), SLOT(contrast_slider_changed(int)));
    connect(ui->brightness_slider, SIGNAL(valueChanged(int)), SLOT(brightness_slider_changed(int)));

    connect(ui->actionHSL, &QAction::triggered, this, [this]() {
        hsl_dialog->show();
        hsl_dialog->raise();      // bring to front if already open
        hsl_dialog->activateWindow();
    });

    connect(hsl_dialog->ui->h_slider, &QSlider::valueChanged,
            this, &MainWindow::hueSliderChanged);
    connect(hsl_dialog->ui->s_slider, &QSlider::valueChanged,
            this, &MainWindow::satSliderChanged);
    connect(hsl_dialog->ui->l_slider, &QSlider::valueChanged,
            this, &MainWindow::lightSliderChanged);

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
}

void MainWindow::apply_changes(){
    if(original_image.isNull() || original_image.isNull()){
        return;
    }

    // QImage local_image = QImage(original_image);

    run_algorithm(original_image, edited_image, AlgorithmArgs{ d_brightness, d_contrast, d_gamma });

    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));
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



/// values from 0 to 359, treat as degrees
void MainWindow::hueSliderChanged(int value){
    current_hsl.h = value;
    apply_hsl();
}

/// values from 0 to 99, treat as percents
void MainWindow::satSliderChanged(int value){
    current_hsl.s = value;
    apply_hsl();
}

/// values from 0 to 99, treat as percents
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
}

