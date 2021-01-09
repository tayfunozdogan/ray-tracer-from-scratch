#include "Plane.h"

Plane::Plane(const Vec3f &normal, const Vec3f &point, const Vec3f &color, const Surface &surface)
        : m_point(point), Object(color, surface)
{
    m_normal = Vec3f(normal).normalize();
}

std::optional<float> Plane::intersect(const Ray &ray) const
{
    std::optional<float> t;
    float denom = ray.dir().dotProduct(m_normal);
    if (std::abs(denom) > 1e-6) {
        float tTemp = (m_point - ray.orig()).dotProduct(m_normal) / denom;
        if (tTemp >= 0.01 && tTemp <= 1000) { t = tTemp; }
    }

    return t;
}

Vec3f Plane::normalAt(const Ray &ray, const float &) const
{
    float denom = ray.dir().dotProduct(m_normal);
    return m_normal * (denom > 0 ? -1.f : 1.f);
}