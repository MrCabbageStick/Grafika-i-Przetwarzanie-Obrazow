#include "algorithm.h"

using PrecaculatedData = uint8_t[256];

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& args){

    // Transform integer <-255,255> values to <0, 1> float values
    // Gamma and contrast should not be negative
    float normalized_gamma = (args.gamma + 255) / 255.0f;
    float normalized_contrast = (args.contrast + 255) / 255.0f;
    float normalized_brightness = args.brightness / 255.0f;

    // Precalculate values to save on resources later
    PrecaculatedData data;
    for(uint16_t val = 0; val <= 255; val++){
        float normalized_val = val / 255.0f;

        // Contrast: value * contrast
        // Brightness: value + brightness
        // Gamma: value ^ (1/gamma)

        // Chain as: gamma(brightness(contrast(value)))
        // then convert back to integer in range <0, 255>

        data[val] = std::clamp(
            std::pow((normalized_val * normalized_contrast + normalized_brightness), 1 / normalized_gamma) * 255,
            0.0f, 255.0f
        );
    }

    for(int y = 0; y < src.height(); y++){
        auto dst_line = reinterpret_cast<QRgb*>(dst.scanLine(y));
        auto src_line = reinterpret_cast<const QRgb*>(src.constScanLine(y));

        for(int x = 0; x < dst.width(); x++){
            // Use helper structure to extract RGBA data from QRgb
            const BGRA* src_pixel = (const BGRA*)(src_line + x);
            BGRA* dst_pixel = (BGRA*)(dst_line + x);

            dst_pixel->r = data[src_pixel->r];
            dst_pixel->g = data[src_pixel->g];
            dst_pixel->b = data[src_pixel->b];
        }
    }
}


