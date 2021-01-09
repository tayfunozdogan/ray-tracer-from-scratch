#ifndef RAY_TRACER_FROM_SCRATCH_SPHERE_H
#define RAY_TRACER_FROM_SCRATCH_SPHERE_H

#include "Object.h"
#include "Ray.h"

class Sphere : public Object {
public:
    Sphere(const Vec3f &center, const float &radius, const Vec3f &color, const Surface &surface);
    std::optional<float> intersect(const Ray &ray) const override;
    Vec3f normalAt(const Ray &ray, const float &t) const override;

    Vec3f m_center;
    float m_radius;
};


#endif //RAY_TRACER_FROM_SCRATCH_SPHERE_H
