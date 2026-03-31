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

    connect(ui->apply_filter_btn, SIGNAL(clicked(bool)), SLOT(apply_filter_pushed()));

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

    edited_image = QImage(original_image);

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

            auto *item = new QTableWidgetItem();

            item->setData(Qt::EditRole, value);

            ui->filter_matrix_table->setItem(y, x, item);
        }
    }

}

FilterMatrix MainWindow::filter_table_to_matrix(){
    size_t matrix_width = ui->filter_matrix_table->columnCount();
    size_t matrix_size = matrix_width * ui->filter_matrix_table->rowCount();

    std::vector<float> matrix(matrix_size);
    int sum = 0;

    for(size_t i = 0; i < matrix_size; i++){
        size_t x = i % matrix_width;
        size_t y = i / matrix_width;

        auto *item = ui->filter_matrix_table->item(y, x);

        int value = item->data(Qt::EditRole).toInt();

        sum += value;
        matrix[i] = value;
    }

    if(sum != 0){
        for(auto &value : matrix){
            value /= sum;
        }
    }

    qDebug() << matrix << "\n";

    return {
        matrix_width,
        matrix,
    };
}

void MainWindow::apply_filter_pushed(){
    if(original_image.isNull()){
        return;
    }

    auto matrix = filter_table_to_matrix();

    run_algorithm(original_image, edited_image, {matrix});

    ui->edited_image->setPixmap(QPixmap::fromImage(edited_image));
}

void MainWindow::test_log(int v){
    qDebug() << "Value: " << v << "\n";
}
