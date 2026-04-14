#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent* event){
    int x = event->pos().x();
    int y = event->pos().y();

    currentTool->toolDown(x, y, &image);
    update();

    // qDebug() << "Pressed: " << x << " " << y << '\n';
}

void MainWindow::mouseMoveEvent(QMouseEvent* event){
    int x = event->pos().x();
    int y = event->pos().y();

    toolLayer.fill(0x000000);
    currentTool->toolMove(x, y, &toolLayer);
    update();

    // qDebug() << "Moved: " << x << " " << y << '\n';
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    int x = event->pos().x();
    int y = event->pos().y();

    currentTool->toolUp(x, y, &image);
    toolLayer.fill(0x000000);
    update();
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    auto key = event->key();

    if(key == Qt::Key_S){
        qDebug() << "Pressed S :3\n";
    }
}

void MainWindow::paintEvent(QPaintEvent* event){
    auto painter = QPainter(this);
    painter.drawImage(0, 0, image);
    painter.drawImage(0, 0, toolLayer);
}

void MainWindow::resizeEvent(QResizeEvent* event){
    image = QImage(event->size().width(), event->size().height(), QImage::Format_RGB32);
    toolLayer = QImage(event->size().width(), event->size().height(), QImage::Format_ARGB32);
    // image.fill(event->size().height() % 256);
}
