#include "Sphere.h"

Sphere::Sphere(const Vec3f &center, const float &radius, const Vec3f &color, const Surface &surface)
        : m_center(center), m_radius(radius), Object(color, surface) {}

std::optional<float> Sphere::intersect(const Ray &ray) const
{
    float t0, t1;
    std::optional<float> t;
    Vec3f L = ray.orig() - m_center;
    float a = ray.dir().dotProduct(ray.dir());
    float b = 2 * L.dotProduct(ray.dir());
    float c = L.dotProduct(L) - m_radius * m_radius;

    float disc = b * b - 4 * a * c;

    if (disc == 0)
        t0 = t1 = b * -0.5f * a;
    else if (disc > 0) {
        float q = -0.5f * (b > 0 ?
                           b + std::sqrt(disc) :
                           b - std::sqrt(disc));
        t0 = q / a;
        t1 = c / q;

        if (t0 > t1) std::swap(t0, t1);
    }
    else
        return t;

    if (t0 < 0) {
        t0 = t1;
        if (t0 < 0) return t;
    }

    t = t0;

    return t;
}

Vec3f Sphere::normalAt(const Ray &ray, const float &t) const
{
    Vec3f pointHit = ray.orig() + t * ray.dir();
    return (pointHit - m_center).normalize();
}