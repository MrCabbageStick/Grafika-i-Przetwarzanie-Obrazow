#include "algorithm.h"

BGRA change_brightness(BGRA src_pixel, uint8_t* data){
    src_pixel.r = data[src_pixel.r];
    src_pixel.g = data[src_pixel.g];
    src_pixel.b = data[src_pixel.b];

    return src_pixel;
}

BGRA change_contrast(BGRA scr_pixel, int d_contrast){
    float normalized_contrast = (d_contrast + 255) / 255.0f;

    float r = scr_pixel.r / 255.0f;
    float g = scr_pixel.g / 255.0f;
    float b = scr_pixel.b / 255.0f;

    return BGRA{
        (unsigned char)std::clamp(b * normalized_contrast * 255, 0.0f, 255.0f),
        (unsigned char)std::clamp(g * normalized_contrast * 255, 0.0f, 255.0f),
        (unsigned char)std::clamp(r * normalized_contrast * 255, 0.0f, 255.0f),
        scr_pixel.a
    };
}

BGRA change_gamma(BGRA scr_pixel, int d_gamma){
    float normalized_gamma = (d_gamma + 255) / 255.0f;

    float r = scr_pixel.r / 255.0f;
    float g = scr_pixel.g / 255.0f;
    float b = scr_pixel.b / 255.0f;

    return BGRA{
        (unsigned char)std::clamp(std::pow(b, 1 / normalized_gamma) * 255, 0.0f, 255.0f),
        (unsigned char)std::clamp(std::pow(g, 1 / normalized_gamma) * 255, 0.0f, 255.0f),
        (unsigned char)std::clamp(std::pow(r, 1 / normalized_gamma) * 255, 0.0f, 255.0f),
        scr_pixel.a
    };
}

uint8_t calculate_brightness(uint8_t value, int brightness){
    return std::clamp(value + brightness, 0, 255);
}

using PrecaculatedData = uint8_t[256];

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& args){

    PrecaculatedData brightness_data;
    for(uint16_t val = 0; val <= 255; val++)
        brightness_data[val] = calculate_brightness(val, args.brightness);



    for(int y = 0; y < src.height(); y++){
        auto dst_line = reinterpret_cast<QRgb*>(dst.scanLine(y));
        auto src_line = reinterpret_cast<const QRgb*>(src.constScanLine(y));

        for(int x = 0; x < dst.width(); x++){
            const BGRA* src_pixel = (const BGRA*)(src_line + x);
            BGRA* dst_pixel = (BGRA*)(dst_line + x);

            // Pamiętaj że cpp jest głupi i robi castowanie domyślne
            // więc uchar + uchar = int
            // BGRA after_changes = change_gamma(
            //     change_brightness(
            //         change_contrast(*src_pixel, args.contrast),
            //     brightness),
            //     args.gamma
            // );
            BGRA after_changes = change_brightness(*src_pixel, brightness_data);

            dst_pixel->r = after_changes.r;
            dst_pixel->g = after_changes.g;
            dst_pixel->b = after_changes.b;
        }
    }
}


