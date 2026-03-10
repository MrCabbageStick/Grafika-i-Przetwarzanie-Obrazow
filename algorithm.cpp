#include "algorithm.h"

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& args){
    const int& brightness = args.brightness;

    for(int y = 0; y < src.height(); y++){
        auto dst_line = reinterpret_cast<QRgb*>(dst.scanLine(y));
        auto src_line = reinterpret_cast<const QRgb*>(src.constScanLine(y));

        for(int x = 0; x < dst.width(); x++){
            const BGRA* src_pixel = (const BGRA*)(src_line + x);
            BGRA* dst_pixel = (BGRA*)(dst_line + x);
            // Następnym razem użyj qRed, qBlue, qGreen żeby wydostać kolory
            // i qRGB żeby złożyć je w kolor RGB.

            // Pamiętaj że cpp jest głupi i robi castowanie domyślne
            // więc uchar + uchar = int

            dst_pixel->r = std::clamp(brightness + src_pixel->r, 0, 255);
            dst_pixel->g = std::clamp(brightness + src_pixel->g, 0, 255);
            dst_pixel->b = std::clamp(brightness + src_pixel->b, 0, 255);
            // dst_pixel->a = std::clamp(dv + src_pixel->a, 0, 255);
        }
    }
}
