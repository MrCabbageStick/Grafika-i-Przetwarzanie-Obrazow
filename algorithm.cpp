#include "algorithm.h"

bool one_pixel_done = false;

BGRA apply_filter_matrix(size_t x, size_t y, const BGRA* src_bytes, size_t src_width, size_t src_height, const FilterMatrix& filter_matrix){
    float sums[3] = {0.0f, 0.0f, 0.0f}; // R, G, B

    size_t matrix_height = filter_matrix.matrix.size() / filter_matrix.width;

    size_t filter_center_x = filter_matrix.width / 2 + 1;
    size_t filter_center_y = matrix_height / 2 + 1;

    for(size_t filter_y = 0; filter_y < matrix_height; filter_y++){
        for(size_t filter_x = 0; filter_x < filter_matrix.width; filter_x++){
            // Offset x by filter_x, but make filter_x relative to filter_center_x
            // clamp it to fit in image limits
            int byte_x = std::clamp<int>(
                (int)x + filter_x - filter_center_x,
                0,
                src_width - 1
            );

            int byte_y = std::clamp<int>(
                (int)y + filter_y - filter_center_y,
                0,
                src_height - 1
            );

            BGRA pixel = src_bytes[byte_x + byte_y * src_width];

            float weight = filter_matrix.matrix[filter_x + filter_y * filter_matrix.width];

            sums[0] += pixel.r / 255.0f * weight;
            sums[1] += pixel.g / 255.0f * weight;
            sums[2] += pixel.b / 255.0f * weight;
        }
    }

    return {
        .b = static_cast<uchar>(std::clamp(sums[2] * 255, 0.0f, 255.0f)),
        .g = static_cast<uchar>(std::clamp(sums[1] * 255, 0.0f, 255.0f)),
        .r = static_cast<uchar>(std::clamp(sums[0] * 255, 0.0f, 255.0f)),
        .a = 255,
    };
}

void run_algorithm(const QImage& src, QImage& dst, const AlgorithmArgs& args){
    auto *src_bytes = reinterpret_cast<const BGRA*>(src.bits());
    auto *dst_bytes = reinterpret_cast<BGRA*>(dst.bits());

    for(size_t y = 0; y < src.height(); y++){
        for(size_t x = 0; x < src.width(); x++){
            dst_bytes[x + y * src.width()] =
                apply_filter_matrix(x, y, src_bytes, src.width(), src.height(), args.filter_matrix);
        }
    }
}
