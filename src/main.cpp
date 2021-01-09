#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <optional>
#include <limits>
#include <algorithm>

#include "geometry.h"
#include "Surface.h"
#include "PointLight.h"
#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"

inline constexpr float bias = 0.001f;

inline Vec3f reflect(const Vec3f &lightDir, const Vec3f &normal)
{
    return normal * 2.f * (normal.dotProduct(lightDir)) - lightDir;
}

struct Options {
    static int width;
    static int height;
    static float fov;
};

int Options::width;
int Options::height;
float Options::fov;

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

Vec3f castRay(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, const std::vector<PointLight> &lights)
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

void render(const std::vector<std::unique_ptr<Object>> &objects, const std::vector<PointLight> &lights)
{
    const float imageAspectRatio = (float)Options::width / (float)Options::height;
    const float scale = tan(Options::fov * 0.5f * (float)pi / 180);

    auto *framebuffer = new Vec3f[Options::width * Options::height];
    Vec3f *pix = framebuffer;

    for (size_t i = 0; i < Options::height; i++) {
        for (size_t k = 0; k < Options::width; k++) {
            float x = (2 * (k + 0.5f) / (float)Options::width - 1) * imageAspectRatio * scale;
            float y = (1 - (2 * (i + 0.5f) / (float)Options::height)) * scale;
            Vec3f dir = Vec3f(x, y, -1).normalize();
            *(pix++) = castRay(Ray(Vec3f(), dir), objects, lights);
        }
    }

    std::ofstream ofs;
    ofs.open("../out.ppm", std::ofstream::out | std::ofstream::binary);
    ofs << "P6\n" << Options::width << " " << Options::height << "\n255\n";

    for (size_t i = 0; i < Options::width * Options::height; ++i) {
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
    auto start = std::chrono::system_clock::now();

    std::vector<std::unique_ptr<Object>> objects;

    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(0, 0, 1), Vec3f(0, 0, -30), Color::ivory(), Surface(Vec2f(0.6, 0.3), 50))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(-2.5, 0, 3), Vec3f(36, 0, -10), Color::pinkRubber(), Surface(Vec2f(0.6, 0.3f), 50))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(-2.5, 0, -3), Vec3f(-12, 0, -30), Color::blue(), Surface(Vec2f(0.6, 0.3), 50))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(0, -5, 6), Vec3f(0, 12, -30), Color::lime(), Surface(Vec2f(0.6, 0.3), 50))));
    objects.push_back(std::unique_ptr<Object>(
            new Plane(Vec3f(0, -5, -6), Vec3f(0, -12, -30), Color::purple(), Surface(Vec2f(0.6, 0.3), 50))));
    objects.push_back(std::unique_ptr<Object>(
            new Sphere(Vec3f(-2,   -2, -8), 1, Color::crimson(), Surface(Vec2f(0.2, 0.3), 50))));
    objects.push_back(std::unique_ptr<Object>(
            new Sphere(Vec3f(-5,   3, -12), 2, Color::chocolate(), Surface(Vec2f(0.6, 0.3), 50.))));
    objects.push_back(std::unique_ptr<Object>(
            new Sphere(Vec3f(4, -3, -18), 3, Color::gold(), Surface(Vec2f(0.3, 0.3), 50))));


    std::vector<PointLight> lights;
    lights.push_back(PointLight(Vec3f(10, 10, 50), 1.f));
    lights.push_back(PointLight(Vec3f(-7, 5, 40), 1.f));
    lights.push_back(PointLight(Vec3f(-14, 14, 30), 1.f));

    Options::width = 1024;
    Options::height = 1024;
    Options::fov = 90;

    render(objects, lights);

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start) / 1000.f;
    std::cout << elapsed.count() << '\n';

    return 0;
}
