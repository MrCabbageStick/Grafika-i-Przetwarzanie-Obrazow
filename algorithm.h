#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QImage>
#include <QDebug>

struct FilterMatrixContext{
    size_t x, y, width, height;
};

struct FilterMatrix{
    size_t width;
    std::vector<float> matrix;
};

struct AlgorithmArgs{
    FilterMatrix filter_matrix;
};

struct BGRA{
    unsigned char b, g, r, a;
};

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& arg);

using MatrixValueProvider = int(*)(FilterMatrixContext ctx);

#endif // ALGORITHM_H
