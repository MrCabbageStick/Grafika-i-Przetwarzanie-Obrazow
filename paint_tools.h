#ifndef PAINT_TOOLS_H
#define PAINT_TOOLS_H

#include <QImage>
#include <QDebug>

struct PaintTool{
    virtual void toolDown(unsigned int x, unsigned int y, QImage* canvas) = 0;
    virtual void toolUp(unsigned int x, unsigned int y, QImage* canvas) = 0;
    virtual void toolMove(unsigned int x, unsigned int y, QImage* canvas) = 0;
};

struct LineTool: public PaintTool{
    unsigned int start_x, start_y;

    void toolDown(unsigned int x, unsigned int y, QImage* canvas) override {
        start_x = x;
        start_y = y;

        canvas->setPixel(x, y, 0xffffff);

        qDebug() << "Set\n";
    }

    void toolUp(unsigned int x, unsigned int y, QImage* canvas) override {
        // Let's "draw" a line from 0,0 to x-x0, y-y0 and then translate it
        int fake_start_x = 0;
        int fake_start_y = 0;

        // dx and dy will be our fake_end_x and fake_end_y
        int dx = x - start_x;
        int dy = y - start_y;

        int d = 2 * dy - dx;
        int bi = 2 * (dy - dx);
        int ai = 2 * dy;

        int pixel_x = fake_start_x;
        int pixel_y = fake_start_y;

        for (; pixel_x != dx; pixel_x++)
        {
            if (d >= 0) {
                d += bi;
                ++pixel_y;
            }
            else d += ai;

            // Translate before draw
            int translated_x = pixel_x + start_x;
            int translated_y = pixel_y + start_y;

            canvas->setPixel(translated_x, translated_y, 0xffffff);
        }
    }

    void toolMove(unsigned int x, unsigned int y, QImage* canvas) override {}
};



#endif // PAINT_TOOLS_H
