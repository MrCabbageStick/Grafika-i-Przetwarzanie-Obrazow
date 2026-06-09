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
#include "ui_transformdialog.h"
#include "transformations.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    histogramDialog = new HistogramDialog(this);

    hsl_dialog = new HslDialog(this);
    lab_dialog = new LabDialog(this);

    transformDialog = new TransformDialog(this);

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

    connect(ui->actionTransformations, &QAction::triggered, this, [this](){
        transformDialog->show();
        transformDialog->raise();      // bring to front if already open
        transformDialog->activateWindow();
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


    connect(transformDialog->ui->degreeSlider, &QSlider::valueChanged, this, &MainWindow::rotationSliderChanged);
    connect(transformDialog->ui->xTranslation, &QSpinBox::valueChanged, this, &MainWindow::xTranslationChanged);
    connect(transformDialog->ui->yTranslation, &QSpinBox::valueChanged, this, &MainWindow::yTranslationChanged);
    connect(transformDialog->ui->xScale, &QSpinBox::valueChanged, this, &MainWindow::xScaleChanged);
    connect(transformDialog->ui->yScale, &QSpinBox::valueChanged, this, &MainWindow::yScaleChanged);
    connect(transformDialog->ui->okCancel, &QDialogButtonBox::accepted, this, &MainWindow::commitTransform);
    connect(transformDialog->ui->okCancel, &QDialogButtonBox::rejected, this, &MainWindow::cancelTransform);



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
    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));

    QPixmap pixmap = QPixmap::fromImage(original_image);

    ui->orignal_image->setPixmap(pixmap);


    transformDialog->ui->xTranslation->setMaximum(edited_image.width());
    transformDialog->ui->xTranslation->setMinimum(-edited_image.width());
    transformDialog->ui->yTranslation->setMaximum(edited_image.height());
    transformDialog->ui->yTranslation->setMinimum(-edited_image.height());


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

void MainWindow::rotationSliderChanged(int value){
    auto radians = qDegreesToRadians((float)(value));
    transform(transform_state.tx, transform_state.ty, radians, transform_state.xScale, transform_state.yScale);
    transform_state.rot = radians;
}

void MainWindow::xTranslationChanged(int value){
    transform(value, transform_state.ty, transform_state.rot, transform_state.xScale, transform_state.yScale);
    transform_state.tx = value;
}

void MainWindow::yTranslationChanged(int value){
    transform(transform_state.tx, value, transform_state.rot, transform_state.xScale, transform_state.yScale);
    transform_state.ty = value;
}

void MainWindow::xScaleChanged(int value){
    auto asFloat = value / 100.0f;
    transform_state.xScale = asFloat;
    transform(transform_state.tx, transform_state.ty, transform_state.rot, asFloat, transform_state.yScale);
}

void MainWindow::yScaleChanged(int value){
    auto asFloat = value / 100.0f;
    transform_state.yScale = asFloat;
    transform(transform_state.tx, transform_state.ty, transform_state.rot, transform_state.xScale, asFloat);
}

void MainWindow::transform(int tx, int ty, float rot, float xScale, float yScale){
    if(editing_copy == nullptr){
        editing_copy = new QImage(edited_image);
    }

    auto source = QImage(*editing_copy);

    edited_image.fill(0x00000000);

    transform::XYPoint origin = {edited_image.width() / 2, edited_image.height() / 2};

    for(int y = 0; y < edited_image.height(); y++){
        // auto src_line = reinterpret_cast<const QRgb*>(source.constScanLine(y));
        auto dst_line = reinterpret_cast<QRgb*>(edited_image.scanLine(y));

        for(int x = 0; x < edited_image.width(); x++){
            // get pixel from source with oposite transformation
            // and put into destination at x, y
            QRgb *dst_pixel = dst_line + x;

            auto new_pixel_pos = transform::transformWithScale({x, y}, origin, {tx, ty}, rot, xScale, yScale);

            QRgb src_pixel;
            if(
                new_pixel_pos.x < edited_image.width() && new_pixel_pos.x >= 0
                && new_pixel_pos.y < edited_image.height() && new_pixel_pos.y >= 0
                ){
                src_pixel = source.pixel(new_pixel_pos.x, new_pixel_pos.y);
            }else{
                src_pixel = 0x00000000;
            }

            *dst_pixel = src_pixel;
        }
    }

    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));

    update_histogram();
}

void MainWindow::commitTransform(){
    transform_state = {0, 0, 0, 1, 1};
    editing_copy = nullptr;

    updateTransformSlider();
}

void MainWindow::cancelTransform(){
    edited_image = *editing_copy;
    delete editing_copy;
    editing_copy = nullptr;
    transform_state = {0, 0, 0, 1, 1};
    update_histogram();
    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));

    updateTransformSlider();
}

void MainWindow::updateTransformSlider(){
    transformDialog->ui->xTranslation->setValue(transform_state.tx);
    transformDialog->ui->yTranslation->setValue(transform_state.ty);
    transformDialog->ui->degreeSlider->setValue(qDegreesToRadians(transform_state.rot));
    transformDialog->ui->xScale->setValue(transform_state.xScale * 100);
    transformDialog->ui->yScale->setValue(transform_state.yScale * 100);
}







