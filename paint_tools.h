#ifndef PAINT_TOOLS_H
#define PAINT_TOOLS_H

#include <cstdlib>
#include <cmath>

#include <QImage>
#include <QDebug>

template<typename T>
T abs(T a){
    return a < 0 ? -a : a;
}

struct PaintTool{
    virtual void toolDown(int x, int y, QImage* canvas) = 0;
    virtual void toolUp(int x, int y, QImage* canvas) = 0;
    virtual void toolMove(int x, int y, QImage* canvas) = 0;
};

struct LineTool: public PaintTool{
    int start_x, start_y;

    void toolDown(int x, int y, QImage* canvas) override {
        start_x = x;
        start_y = y;

        canvas->setPixel(x, y, 0xffffff);

        qDebug() << "Set\n";
    }

    void toolUp(int dx, int dy, QImage* canvas) override {
        int x = start_x;
        int y = start_y;

        int step_x = (dx - start_x) < 0 ? -1 : 1;
        int step_y = (dy - start_y) < 0 ? -1 : 1;

        int delta_x = abs(dx - start_x);
        int delta_y = abs(dy - start_y);

        int error = delta_x - delta_y;

        while (true) {
            canvas->setPixel(x, y, 0xffffffff);

            if (x == dx && y == dy) break;

            int error2 = error * 2;

            if (delta_y == 0 || error2 > -delta_y) {
                error -= delta_y;
                x += step_x;
            }
            if (delta_x == 0 || error2 < delta_x) {
                error += delta_x;
                y += step_y;
            }
        }
    }

    void toolMove(int x, int y, QImage* canvas) override {
        toolUp(x, y, canvas);
    }
};



struct CircleTool: public PaintTool{
    int start_x, start_y, radius;

    virtual void toolDown(int x, int y, QImage* canvas){
        start_x = x;
        start_y = y;
        radius = 0;
    }

    virtual void toolUp(int dx, int dy, QImage* canvas){
        int x_length = abs(dx - start_x);
        int y_length = abs(dy - start_y);
        radius = sqrt(x_length * x_length + y_length * y_length);

        int d = 5 - 4 * radius;
        int x = 0;
        int y = radius;
        int deltaA = (-2 * radius + 5) * 4;
        int deltaB = 3 * 4;

        while (x <= y) {
            drawAllOctans(x, y, canvas);

            if (d > 0) {
                d += deltaA;
                y--;
                x++;
                deltaA += 4 * 4;
                deltaB += 2 * 4;
            } else {
                d += deltaB;
                x++;
                deltaA += 2 * 4;
                deltaB += 2 * 4;
            }
        }
    }

    virtual void toolMove(int x, int y, QImage* canvas){
        toolUp(x, y, canvas);
    }

    void drawAllOctans(int x, int y, QImage* canvas){
        canvas->setPixel(start_x + x, start_y + y, 0xffffffff);
        canvas->setPixel(start_x - x, start_y + y, 0xffffffff);
        canvas->setPixel(start_x + x, start_y - y, 0xffffffff);
        canvas->setPixel(start_x - x, start_y - y, 0xffffffff);
        canvas->setPixel(start_x + y, start_y + x, 0xffffffff);
        canvas->setPixel(start_x - y, start_y + x, 0xffffffff);
        canvas->setPixel(start_x + y, start_y - x, 0xffffffff);
        canvas->setPixel(start_x - y, start_y - x, 0xffffffff);
    }
};


#endif // PAINT_TOOLS_H
