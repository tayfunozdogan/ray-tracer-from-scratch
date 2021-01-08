#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <optional>
#include <limits>
#include <algorithm>

#include "geometry.h"

constexpr float bias = 0.001f;

struct Surface {
    Surface(const Vec2f &albedo, const float &spec) : m_albedo(albedo), m_specularExponent(spec) {}
    Surface() : m_albedo(1, 0), m_specularExponent() {}
    Vec2f m_albedo;
    float m_specularExponent;
};

struct Light {
    Light(const Vec3f &pos, const float &intensity) : m_position(pos), m_intensity(intensity) {}
    Vec3f m_position;
    float m_intensity;
};

class Ray {
public:
    Ray(const Vec3f &orig, const Vec3f &dir) : m_origin(orig), m_direction(dir) {}
    Vec3f orig() const { return m_origin; }
    Vec3f dir() const { return m_direction; }

private:
    Vec3f m_origin, m_direction;
};

class Object {
public:
    explicit Object(const Vec3f &color, const Surface &surface) : m_color(color / 255.f), m_surface(surface) {}
    virtual std::optional<float> intersect(const Ray &) const = 0;
    virtual Vec3f normalAt(const Ray &, const float &) const = 0;

    Vec3f m_color;
    Surface m_surface;
};

class Sphere : public Object {
public:
    Sphere(const Vec3f &center, const float &radius, const Vec3f &color, const Surface &surface)
            : m_center(center), m_radius(radius), Object(color, surface) {}

    std::optional<float> intersect(const Ray &ray) const override
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

    Vec3f normalAt(const Ray &ray, const float &t) const override
    {
        Vec3f pointHit = ray.orig() + t * ray.dir();
        return (pointHit - m_center).normalize();
    }

    Vec3f m_center;
    float m_radius;
};

class Plane : public Object {
public:
    Plane(const Vec3f &normal, const Vec3f &point, const Vec3f &color, const Surface &surface)
            : m_point(point), Object(color, surface)
    {
        m_normal = Vec3f(normal).normalize();
    }

    std::optional<float> intersect(const Ray &ray) const override
    {
        std::optional<float> t;
        float denom = ray.dir().dotProduct(m_normal);
        if (std::abs(denom) > 1e-6) {
            float tTemp = (m_point - ray.orig()).dotProduct(m_normal) / denom;
            if (tTemp >= 0.01 && tTemp <= 1000) {
                t = tTemp;
            }
        }

        return t;
    }

    Vec3f normalAt(const Ray &ray, const float &) const override
    {
        float denom = ray.dir().dotProduct(m_normal);
        return m_normal * (denom > 0 ? -1.f : 1.f);
    }

    Vec3f m_normal, m_point;
};

Vec3f reflect(const Vec3f &lightDir, const Vec3f &normal)
{
    return normal * 2.f * (normal.dotProduct(lightDir)) - lightDir;
}

bool trace(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, float &tNear, const Object *&hitObject)
{
    for (const auto & object : objects) {
        auto t = object->intersect(ray);
        if (t.has_value() && tNear > t.value()) {
            hitObject = object.get();
            tNear = t.value();
        }
    }

    return (hitObject != nullptr);
}

Vec3f castRay(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, const std::vector<Light> &lights)
{
    float t = std::numeric_limits<float>::max();
    const Object *hitObject = nullptr;

    if (trace(ray, objects, t, hitObject)) {
        Vec3f pointHit = ray.orig() + t * ray.dir();
        Vec3f normalRay = hitObject->normalAt(ray, t);
        float diffuseLightInt = 0, specularLightInt = 0;

        for (auto &light : lights) {
            Vec3f lightDir = (light.m_position - pointHit).normalize();
            Vec3f shadowOrig = pointHit + normalRay * bias;
            const Object *shadowHitObject = nullptr;
            float tShadow = std::numeric_limits<float>::max();

            const Ray shadowRay(shadowOrig, lightDir);
            bool vis = !trace(shadowRay, objects, tShadow, shadowHitObject);
            diffuseLightInt += vis * light.m_intensity * std::max(0.f, normalRay.dotProduct(lightDir));
            specularLightInt += vis * std::powf(std::max(0.f, reflect(lightDir, normalRay).dotProduct(-ray.dir())),
                                                hitObject->m_surface.m_specularExponent) * light.m_intensity;
        }

        return hitObject->m_color * diffuseLightInt * hitObject->m_surface.m_albedo[0] +
               Vec3f(1.f, 1.f, 1.f) * specularLightInt * hitObject->m_surface.m_albedo[1];
    }

    return Vec3f(0.69f, 0.13f, 0.13f);
}

void render(const std::vector<std::unique_ptr<Object>> &objects, const std::vector<Light> &lights)
{
    const int width = 2048;
    const int height = 1536;
    const float fov = 90;
    const float imageAspectRatio = width / (float)height;
    const float scale = tan(fov * 0.5f * (float)pi / 180);

    auto *framebuffer = new Vec3f[width * height];
    Vec3f *pix = framebuffer;

    for (size_t i = 0; i < height; i++) {
        for (size_t k = 0; k < width; k++) {
            float x = (2 * (k + 0.5f) / width - 1) * imageAspectRatio * scale;
            float y = (1 - (2 * (i + 0.5f) / height)) * scale;
            Vec3f dir = Vec3f(x, y, -1).normalize();
            *(pix++) = castRay(Ray(Vec3f(), dir), objects, lights);
        }
    }

    std::ofstream ofs;
    ofs.open("../out.ppm", std::ofstream::out | std::ofstream::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";

    for (size_t i = 0; i < width * height; ++i) {
        Vec3f &p = framebuffer[i];
        float max = std::max(p.x, std::max(p.y, p.z));
        if (max > 1) p = p * (1.f / max);
        char r = (char)(255 * std::max(0.f, std::min(1.f, p.x)));
        char g = (char)(255 * std::max(0.f, std::min(1.f, p.y)));
        char b = (char)(255 * std::max(0.f, std::min(1.f, p.z)));
        ofs << r << g << b;
    }

    ofs.close();

    delete[] framebuffer;
}

int main()
{
    std::vector<std::unique_ptr<Object>> objects;

    Vec3f ivory     (102.f, 102.f,  76.5f);
    Vec3f pinkRubber(76.5f,  51.f,  51.f);
    Vec3f lime      (50.f,  205.f,  50.f);
    Vec3f blue      (51.f,  76.5f,  255.f);
    Vec3f purple    (127.5f,  0.f,  127.5f);
    Vec3f chocolate (210.f, 105.f,  30.f);
    Vec3f turquoise (72.f,  209.f,  204.f);
    Vec3f crimson   (220.f,  20.f,  60.f);
    Vec3f gold      (255.f, 215.f,  0.f);
    Vec3f violet    (138.f,  43.f,  226.f);

    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(0, 0, 1), Vec3f(0, 0, -30), ivory, Surface(Vec2f(0.6f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(-2.5, 0, 3), Vec3f(36.f, 0, -10), pinkRubber, Surface(Vec2f(0.6f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(-2.5, 0, -3), Vec3f(-12.f, 0, -30), blue, Surface(Vec2f(0.6f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(0, -5.f, 6), Vec3f(0.f, 12.f, -30), lime, Surface(Vec2f(0.6f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(0, -5.f, -6), Vec3f(0, -12.f, -30), purple, Surface(Vec2f(0.6f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Sphere(Vec3f(-2,   -2, -8), 1, crimson, Surface(Vec2f(0.2f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Sphere(Vec3f(-5,   3, -12), 2, chocolate, Surface(Vec2f(0.6f, 0.3f), 50.f))));
    objects.push_back(std::unique_ptr<Object>(
            new Sphere(Vec3f(4, -3, -18), 3, gold, Surface(Vec2f(0.3f, 0.3f), 50.f))));


    std::vector<Light> lights;
    lights.push_back(Light(Vec3f(10, 10,  50), 1.f));
    lights.push_back(Light(Vec3f( -7, 5, 40), 1.f));
    lights.push_back(Light(Vec3f( -14, 14,  30), 1.f));

    render(objects, lights);

    return 0;
}
