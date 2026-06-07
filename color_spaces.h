#ifndef COLOR_SPACES_H
#define COLOR_SPACES_H

#include "algorithm.h"

struct Hsl{
    float h, s, l;
};

Hsl rgb2hsl(BGRA rgb){
    float r = rgb.r / 255.0;
    float g = rgb.g / 255.0;
    float b = rgb.b / 255.0;

    float min_value = std::min({r, g, b});
    float max_value = std::max({r, g, b});

    float luminace = (min_value + max_value) / 2;

    float max_sub_min = (max_value - min_value);

    // If min == max => saturation = 0
    float saturation = 0;
    if(min_value != max_value){
        if(luminace <= 0.5) saturation = max_sub_min / (min_value + max_value);
        else saturation = max_sub_min / (2.0 - max_value - min_value);
    }

    float hue;

    if(max_value == r) hue = (g - b) / max_sub_min;
    else if(max_value == g) hue = 2.0 + (b - r) / max_sub_min;
    else hue = 4.0 + (r - g) / max_sub_min;

    // Convert to degrees
    hue *= 60;

    if(hue < 0) hue += 360.0;

    return {.h = hue, .s = saturation, .l = luminace};
}

BGRA hsl2rgb(Hsl hsl){
    if(hsl.s == 0.0){
        u_int8_t value = (u_int8_t)(hsl.l * 255);
        return {.b = value, .g = value, .r = value, .a = 255};
    }

    float temp1;
    if(hsl.l < 0.5) temp1 = hsl.l * (1.0 + hsl.s);
    else temp1 = hsl.l + hsl.s - hsl.l * hsl.s;

    float temp2 = 2 * hsl.l - temp1;

    float hue_fraction = hsl.h / 360;

    float temp_channels[3] = {
        hue_fraction + 0.333f, // red
        hue_fraction, // green
        hue_fraction - 0.333f, // blue
    };

    // Bounds check
    for(float &value : temp_channels){
        if(value < 0) value += 1; else if(value > 1) value -= 1;
    }

    float color_channels[3] = {0, 0, 0};

    for(int channel_i = 0; channel_i < 3; channel_i++){
        float temp = temp_channels[channel_i];
        float &channel = color_channels[channel_i];

        if(6 * temp < 1) channel = temp2 + (temp1 - temp2) * 6 * temp;
        else if(2 * temp < 1) channel = temp1;
        else if(3 * temp < 2) channel = temp2 + (temp1 - temp2) * (0.666 - temp) * 6;
        else if(3 * temp >= 2) channel = temp2;

        channel *= 255;
    }

    return {
        .b = (u_int8_t)color_channels[2],
        .g = (u_int8_t)color_channels[1],
        .r = (u_int8_t)color_channels[0],
        .a = 255,
    };
}


struct XYZ {
    float x, y, z;
};

XYZ rgb2xyz(BGRA rgb) {
    // Normalize to 0-1
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    // Apply sRGB gamma correction
    r = (r <= 0.04045f) ? r / 12.92f : powf((r + 0.055f) / 1.055f, 2.4f);
    g = (g <= 0.04045f) ? g / 12.92f : powf((g + 0.055f) / 1.055f, 2.4f);
    b = (b <= 0.04045f) ? b / 12.92f : powf((b + 0.055f) / 1.055f, 2.4f);

    // Convert to XYZ (D65 illuminant)
    float x = r * 0.4124564f + g * 0.3575761f + b * 0.1804375f;
    float y = r * 0.2126729f + g * 0.7151522f + b * 0.0721750f;
    float z = r * 0.0193339f + g * 0.1191920f + b * 0.9503041f;

    return {.x = x, .y = y, .z = z};
}

BGRA xyz2rgb(XYZ xyz) {
    // Convert to linear RGB (D65 illuminant)
    float r =  xyz.x * 3.2404542f + xyz.y * -1.5371385f + xyz.z * -0.4985314f;
    float g =  xyz.x * -0.9692660f + xyz.y * 1.8760108f + xyz.z * 0.0415560f;
    float b =  xyz.x * 0.0556434f + xyz.y * -0.2040259f + xyz.z * 1.0572252f;

    // Apply sRGB gamma compression
    r = (r <= 0.0031308f) ? r * 12.92f : 1.055f * powf(r, 1.0f / 2.4f) - 0.055f;
    g = (g <= 0.0031308f) ? g * 12.92f : 1.055f * powf(g, 1.0f / 2.4f) - 0.055f;
    b = (b <= 0.0031308f) ? b * 12.92f : 1.055f * powf(b, 1.0f / 2.4f) - 0.055f;

    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);

    return {
        .b = (uint8_t)(b * 255.0f),
        .g = (uint8_t)(g * 255.0f),
        .r = (uint8_t)(r * 255.0f),
        .a = 255
    };
}


struct LAB{
   float l, a, b;
};

LAB xyz2lab(XYZ xyz){
    constexpr float temperature[3] = {0.9505, 1.000, 1.0891};

    float x_over_x0_croot = rootn(xyz.x / temperature[0], 3);
    float y_over_y0_croot = rootn(xyz.y / temperature[1], 3);
    float z_over_z0_croot = rootn(xyz.z / temperature[2], 3);

    return {
        .l = 116 * y_over_y0_croot - 16,
        .a = 500 * (x_over_x0_croot - y_over_y0_croot),
        .b = 200 * (y_over_y0_croot - z_over_z0_croot),
    };
}

XYZ lab2xyz(LAB lab) {
    constexpr float temperature[3] = {0.9505, 1.000, 1.0891};

    float y_part = (lab.l + 16.0) / 116.0f;
    float x_part = (lab.a / 500.0f) + y_part;
    float z_part = y_part - (lab.b / 200.0f);

    return {
        .x = powf(x_part, 3) * temperature[0],
        .y = powf(y_part, 3) * temperature[1],
        .z = powf(z_part, 3) * temperature[2],
    };
}

LAB rgb2lab(BGRA rgb){
    return xyz2lab(rgb2xyz(rgb));
}

BGRA lab2rgb(LAB lab){
    return xyz2rgb(lab2xyz(lab));
}

#endif // COLOR_SPACES_H















