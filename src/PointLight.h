#ifndef RAY_TRACER_FROM_SCRATCH_POINTLIGHT_H
#define RAY_TRACER_FROM_SCRATCH_POINTLIGHT_H

#include "geometry.h"

struct PointLight {
    PointLight(const Vec3f &pos, const float &intensity) : m_position(pos), m_intensity(intensity) {}

    Vec3f m_position;
    float m_intensity;
};

#endif //RAY_TRACER_FROM_SCRATCH_POINTLIGHT_H
