#ifndef RAY_TRACER_FROM_SCRATCH_PLANE_H
#define RAY_TRACER_FROM_SCRATCH_PLANE_H

#include "Object.h"
#include "Ray.h"

class Plane : public Object {
public:
    Plane(const Vec3f &normal, const Vec3f &point, const Vec3f &color, const Surface &surface);
    std::optional<float> intersect(const Ray &ray) const override;
    Vec3f normalAt(const Ray &ray, const float &) const override;

    Vec3f m_normal, m_point;
};


#endif //RAY_TRACER_FROM_SCRATCH_PLANE_H
