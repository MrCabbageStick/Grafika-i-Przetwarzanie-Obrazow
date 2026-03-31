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

    connect(ui->radius_slider, SIGNAL(valueChanged(int)), SLOT(filter_radius_changed(int)));
    connect(ui->radius_input, SIGNAL(valueChanged(int)), SLOT(filter_radius_changed(int)));
    connect(ui->template_select, SIGNAL(currentIndexChanged(int)), SLOT(filter_template_changed(int)));

    filter_radius = 1;
    get_provider_for_index(0);
    update_filter_table();

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

void MainWindow::get_provider_for_index(int index){
    switch(index){
        case 0: {
            matrix_value_provider = [](auto _ctx){ return 0; };
            break;
        }
        default: {
            matrix_value_provider = [](auto _ctx){ return 1; };
            break;
        }
    }
}

void MainWindow::filter_radius_changed(int value){
    filter_radius = value;
    sync_filter_radius();
    update_filter_table();
}

void MainWindow::sync_filter_radius(){
    ui->radius_input->setValue(filter_radius);
    ui->radius_slider->setValue(filter_radius);
}

void MainWindow::filter_template_changed(int index){
    get_provider_for_index(index);
    update_filter_table();
}

void MainWindow::update_filter_table(){
    ui->filter_matrix_table->clear();

    auto size = 2 * filter_radius + 1;

    ui->filter_matrix_table->setRowCount(size);
    ui->filter_matrix_table->setColumnCount(size);


    for(size_t x = 0; x < size; x++){
        ui->filter_matrix_table->setColumnWidth(x, 3);
        for(size_t y = 0; y < size; y++){
            auto value = matrix_value_provider(FilterMatrixContext{x, y, size, size});

            // Create a standard item instead of a heavy widget
            auto *item = new QTableWidgetItem();

            // Passing an int automatically tells Qt to use a SpinBox when editing!
            item->setData(Qt::EditRole, value);

            ui->filter_matrix_table->setItem(y, x, item);
        }
    }

}

void MainWindow::test_log(int v){
    qDebug() << "Value: " << v << "\n";
}
