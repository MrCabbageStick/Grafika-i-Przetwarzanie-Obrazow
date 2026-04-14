#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QImage>
#include <QPainter>

#include "paint_tools.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QImage image;
    QImage toolLayer;
    PaintTool* currentTool = new LineTool();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent* event);

    void resizeEvent(QResizeEvent* event);

    void paintEvent(QPaintEvent* event);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
