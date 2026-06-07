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

#endif // COLOR_SPACES_H















