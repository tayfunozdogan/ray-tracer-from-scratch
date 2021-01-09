#ifndef RAY_TRACER_FROM_SCRATCH_RAY_H
#define RAY_TRACER_FROM_SCRATCH_RAY_H

#include "geometry.h"

class Ray {
public:
    Ray(const Vec3f &orig, const Vec3f &dir) : m_origin(orig), m_direction(dir) {}

    Vec3f orig() const { return m_origin; }
    Vec3f dir() const { return m_direction; }

private:
    Vec3f m_origin, m_direction;
};
#endif //RAY_TRACER_FROM_SCRATCH_RAY_H
