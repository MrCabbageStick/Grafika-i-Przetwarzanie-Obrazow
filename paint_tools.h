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

        int fake_dx = dx;
        int fake_dy = dy;

        // If drawing more vertical swap x and y
        if(dy > dx){
            fake_dx = dy;
            fake_dy = dx;
        }

        // If drawing to smaller x or y, flip it
        int y_multiplier = 1;
        int x_multiplier = 1;

        if(fake_dx < 0) x_multiplier = -1;
        if(fake_dy < 0) y_multiplier = -1;

        fake_dx *= x_multiplier;
        fake_dy *= y_multiplier;

        int d = 2 * fake_dy - fake_dx;
        int bi = 2 * (fake_dy - fake_dx);
        int ai = 2 * fake_dy;

        int pixel_x = fake_start_x;
        int pixel_y = fake_start_y;

        for (; pixel_x != fake_dx; pixel_x++)
        {
            if (d >= 0) {
                d += bi;
                ++pixel_y;
            }
            else d += ai;

            // Unswap x and y
            int unswapped_x = pixel_x * x_multiplier;
            int unswapped_y = pixel_y * y_multiplier;

            if(dy > dx){
                unswapped_x = pixel_y * y_multiplier;
                unswapped_y = pixel_x * x_multiplier;
            }

            // Translate before draw
            int translated_x = unswapped_x + start_x;
            int translated_y = unswapped_y + start_y;


            canvas->setPixel(translated_x, translated_y, 0xffffffff);
        }
    }

    void toolMove(unsigned int x, unsigned int y, QImage* canvas) override {
        toolUp(x, y, canvas);
    }
};



#endif // PAINT_TOOLS_H
