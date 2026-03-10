#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QImage>

struct AlgorithmArgs{
    int brightness;
};

struct BGRA{
    unsigned char b, g, r, a;
};

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& arg);

#endif // ALGORITHM_H
