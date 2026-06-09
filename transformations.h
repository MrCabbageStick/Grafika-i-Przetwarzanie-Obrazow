#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <cmath>

namespace transform{

struct XYPoint{
    int x, y;
};

XYPoint rotate(XYPoint point, XYPoint origin, float radians){
    // Make point's origin 0, 0
    XYPoint originless = {.x = point.x - origin.x, .y = point.y - origin.y};

    XYPoint rotated = {
        .x = (int)(cos(radians) * originless.x - sin(radians) * originless.y),
        .y = (int)(sin(radians) * originless.x + cos(radians) * originless.y),
    };

    // Make point's origin origin
    return {
        .x = rotated.x + origin.x,
        .y = rotated.y + origin.y,
    };
}

XYPoint transform(XYPoint point, XYPoint origin, XYPoint translation, float radians){
    float c = std::cos(radians);
    float s = std::sin(radians);

    int ox = origin.x;
    int oy = origin.y;

    // Undo translation
    float px = point.x - translation.x;
    float py = point.y - translation.y;

    // Undo rotation around origin (center)
    px -= ox;
    py -= oy;

    float rx = px * c - py * (-s);  // inverse rotation: transpose of rot matrix
    float ry = px * (-s) + py * c;

    rx += ox;
    ry += oy;

    return { (int)rx, (int)ry };
}

XYPoint transformWithScale(XYPoint point, XYPoint origin, XYPoint translation, float radians, float xScale, float yScale) {
    float c = std::cos(radians);
    float s = std::sin(radians);

    int ox = origin.x;
    int oy = origin.y;

    // Undo translation
    float px = point.x - translation.x;
    float py = point.y - translation.y;

    // Undo rotation around origin
    px -= ox;
    py -= oy;

    float rx = px * c + py * s;  // inverse rotation (transposed)
    float ry = -px * s + py * c;

    // Undo scale around origin
    rx /= xScale;
    ry /= yScale;

    rx += ox;
    ry += oy;

    return { (int)rx, (int)ry };
}

}

#endif // TRANSFORMATIONS_H
