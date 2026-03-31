#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <qspinbox.h>

#include "algorithm.h"

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
    uint filter_radius;
    MatrixValueProvider matrix_value_provider;
    std::vector<std::unique_ptr<QSpinBox>> table_inputs;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void sync_filter_radius();

    void get_provider_for_index(int index);

    void update_filter_table();

public slots:
    void load_image_pushed();

    void filter_radius_changed(int value);

    void filter_template_changed(int index);

    void test_log(int v);
};
#endif // MAINWINDOW_H
