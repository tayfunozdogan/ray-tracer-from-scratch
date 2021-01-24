#ifndef RAY_TRACER_FROM_SCRATCH_SCENE_H
#define RAY_TRACER_FROM_SCRATCH_SCENE_H
#include <cmath>
#include <vector>
#include <fstream>
#include <optional>
#include <limits>
#include <algorithm>
#include <random>
#include "geometry.h"
#include "Ray.h"
#include "Object.h"
#include "PointLight.h"
#include "Settings.h"
#include "Color.h"

inline constexpr float bias = 0.001f;

class Scene {
    Settings settings;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
    std::uniform_real_distribution<> disRGB;

public:
    explicit Scene(const Settings &opt);

    void render(const std::vector<std::unique_ptr<Object>> &objects, const std::vector<PointLight> &lights);

    Vec3f getSphereRandomPosition()
    {
        return Vec3f{dis(gen) * 15, dis(gen) * 15, ((dis(gen) + 0.5f) * -15) - 10 };
    }

    float getSphereRandRadius()
    {
        return dis(gen) + 1.2f;
    }

    Vec3f getRandomColor()
    {
        return Vec3f(disRGB(gen), disRGB(gen), disRGB(gen));
    }

    void setRandomSpheres(std::vector<std::unique_ptr<Object>> &objects, int numberOfSpheres);

private:
    static Vec3f reflect(const Vec3f &lightDir, const Vec3f &normal)
    {
        return normal * 2.f * (normal.dotProduct(lightDir)) - lightDir;
    }

    bool trace(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, float &tNear, const Object *&hitObject);

    Vec3f castRay(const Ray &ray, const std::vector<std::unique_ptr<Object>> &objects, const std::vector<PointLight> &lights);
};


#endif //RAY_TRACER_FROM_SCRATCH_SCENE_H
