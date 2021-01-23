#include <iostream>
#include <vector>
#include <optional>

#include "geometry.h"
#include "Surface.h"
#include "PointLight.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Settings.h"
#include "Color.h"
#include "Scene.h"

void run(const Settings &options)
{
    Scene scene(options);
    std::vector<std::unique_ptr<Object>> objects;

    scene.setRandomSpheres(objects, 8);

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

    std::vector<PointLight> lights;
    lights.push_back(PointLight(Vec3f(20, 20, 20), 1.f));
    lights.push_back(PointLight(Vec3f(-7, -7, 10), 1.f));
    lights.push_back(PointLight(Vec3f(-14, 14, 30), 1.f));

    scene.render(objects, lights);
}

int main()
{
    Settings settings{};
    settings.width = 2048;
    settings.height = 2048;
    settings.fov = 90;

    auto start = std::chrono::system_clock::now();


    run(settings);

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start) / 1000.f;
    std::cout << elapsed.count() << " saniye sürdü.. \n";

    return 0;
}
