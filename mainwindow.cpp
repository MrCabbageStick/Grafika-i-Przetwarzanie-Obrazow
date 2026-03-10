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
}

void MainWindow::apply_changes(){
    if(original_image.isNull()){
        return;
    }

    QImage local_image = QImage(original_image);

    run_algorithm(original_image, local_image, AlgorithmArgs{ d_brightness, d_contrast, d_gamma });

    ui->edited_image->setPixmap(QPixmap::fromImage(local_image));
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


