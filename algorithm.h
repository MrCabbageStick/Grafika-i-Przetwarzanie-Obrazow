#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QImage>

struct FilterMatrixContext{
    size_t x, y, width, height;
};

struct AlgorithmArgs{
    int brightness;
};

struct BGRA{
    unsigned char b, g, r, a;
};

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& arg);

using MatrixValueProvider = int(*)(FilterMatrixContext ctx);

#endif // ALGORITHM_H
