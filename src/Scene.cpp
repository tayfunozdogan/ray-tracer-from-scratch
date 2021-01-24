#include "Scene.h"
#include "Sphere.h"

Scene::Scene(const Settings &sett) : settings(sett), gen(std::random_device()()), dis(-0.5f, 0.5f), disRGB(0, 255)
{
    auto start = std::chrono::system_clock::now();
    gen.seed(start.time_since_epoch().count());
}

bool Scene::trace(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, float &tNear, const Object *&hitObject)
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

Vec3f Scene::castRay(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, const std::vector<PointLight> &lights)
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

void Scene::render(const std::vector<std::unique_ptr<Object>> &objects, const std::vector<PointLight> &lights)
{
    const float imageAspectRatio = (float)settings.width / (float)settings.height;
    const float scale = tan(settings.fov * 0.5f * (float)pi / 180);

    auto *framebuffer = new Vec3f[settings.width * settings.height];
    Vec3f *pix = framebuffer;

    for (size_t i = 0; i < settings.height; i++) {
        for (size_t k = 0; k < settings.width; k++) {
            float x = (2 * (k + 0.5f) / (float)settings.width - 1) * imageAspectRatio * scale;
            float y = (1 - (2 * (i + 0.5f) / (float)settings.height)) * scale;
            Vec3f dir = Vec3f(x, y, -1).normalize();
            *(pix++) = castRay(Ray(Vec3f(), dir), objects, lights);
        }
    }

    std::ofstream ofs;
    ofs.open("../out.ppm", std::ofstream::out | std::ofstream::binary);
    ofs << "P6\n" << settings.width << " " << settings.height << "\n255\n";

    for (size_t i = 0; i < settings.width * settings.height; ++i) {
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

void Scene::setRandomSpheres(std::vector<std::unique_ptr<Object>> &objects, int numberOfSpheres = 1)
{
    for (int i = 0; i < numberOfSpheres; ++i) {
        objects.push_back(std::unique_ptr<Object>(
                new Sphere(this->getSphereRandomPosition(), this->getSphereRandRadius(), this->getRandomColor(), Surface(Vec2f(0.4, 0.6), 50))));
    }
}
