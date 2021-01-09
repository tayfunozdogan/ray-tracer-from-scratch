#ifndef RAY_TRACER_FROM_SCRATCH_OBJECT_H
#define RAY_TRACER_FROM_SCRATCH_OBJECT_H

#include <optional>
#include "geometry.h"
#include "Surface.h"
#include "Ray.h"

class Object {
public:
    explicit Object(const Vec3f &color, const Surface &surface) : m_color(color / 255.f), m_surface(surface) {}
    virtual std::optional<float> intersect(const Ray &) const = 0;
    virtual Vec3f normalAt(const Ray &, const float &) const = 0;

    Vec3f m_color;
    Surface m_surface;
};
#endif //RAY_TRACER_FROM_SCRATCH_OBJECT_H
