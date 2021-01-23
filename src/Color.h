#ifndef RAY_TRACER_FROM_SCRATCH_COLOR_H
#define RAY_TRACER_FROM_SCRATCH_COLOR_H

#include "geometry.h"

struct Color {
    static Vec3f ivory() { return Vec3f(102, 102,  76); }
    static Vec3f pinkRubber() { return Vec3f(76,  51,  51); }
    static Vec3f lime() { return Vec3f(50,  205,  50); }
    static Vec3f blue() { return Vec3f(51,  76,  255); }
    static Vec3f purple() { return Vec3f(127,  0,  127); }
    static Vec3f chocolate() { return Vec3f(210, 105,  30); }
    static Vec3f turquoise() { return Vec3f(72,  209,  204); }
    static Vec3f crimson() { return Vec3f(220,  20,  60); }
    static Vec3f gold() { return Vec3f(255, 215,  0); }
    static Vec3f violet() { return Vec3f(138,  43,  226); }
};

#endif //RAY_TRACER_FROM_SCRATCH_COLOR_H
