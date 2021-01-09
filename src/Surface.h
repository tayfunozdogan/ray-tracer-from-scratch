#ifndef RAY_TRACER_FROM_SCRATCH_SURFACE_H
#define RAY_TRACER_FROM_SCRATCH_SURFACE_H

#include "geometry.h"

struct Surface {
    Surface(const Vec2f &albedo, const float &spec) : m_albedo(albedo), m_specularExponent(spec) {}
    Surface() : m_albedo(1, 0), m_specularExponent() {}

    Vec2f m_albedo;
    float m_specularExponent;
};

#endif //RAY_TRACER_FROM_SCRATCH_SURFACE_H
