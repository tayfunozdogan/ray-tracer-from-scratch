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
	Surface(const Vec2f &a, const float &spec) : albedo(a), specularExponent(spec) {}
	Surface() : albedo(1, 0), specularExponent() {}
	Vec2f albedo;
	float specularExponent;
};

struct Light {
	Light(const Vec3f &p, const float &i) : position(p), intensity(i) {}
	Vec3f position;
	float intensity;
};

class Sphere {
public:
	Sphere(const Vec3f &c, const float &r, const Vec3f &col, const Surface &s)
		: center(c), radius(r), color(col), surface(s) {}

	std::optional<float> intersect(const Vec3f &orig, const Vec3f &dir) const
	{
		float t0, t1;
		std::optional<float> t;
		Vec3f L = orig - center;
		float a = dir.dotProduct(dir);
		float b = 2 * L.dotProduct(dir);
		float c = L.dotProduct(L) - radius * radius;

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

	Vec3f center, color;
	float radius;
	Surface surface;
};

Vec3f reflect(const Vec3f &lightDir, const Vec3f &normal)
{
	return normal * 2.f * (normal.dotProduct(lightDir)) - lightDir;
}

bool trace(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres, float &tNear, const Sphere *&hitSphere)
{
	for (auto &sphere : spheres) {
		auto t = sphere.intersect(orig, dir);
		if (t.has_value() && tNear > t.value()) {
			hitSphere = &sphere;
			tNear = t.value();
		}
	}

	return (hitSphere != nullptr);
}

Vec3f castRay(const Vec3f &orig, Vec3f &dir, const std::vector<Sphere> &spheres, const std::vector<Light> &lights)
{
	float t = std::numeric_limits<float>::max();
	const Sphere *hitSphere = nullptr;

	if (trace(orig, dir, spheres, t, hitSphere)) {
		Vec3f pointHit = orig + t * dir;
		Vec3f normalRay = (pointHit - hitSphere->center).normalize();
		float diffuseLightInt = 0, specularLightInt = 0;

		for (auto &light : lights) {
			Vec3f lightDir = (light.position - pointHit).normalize();
			Vec3f shadowOrig = pointHit + normalRay * bias;
			const Sphere *shadowHitSphere = nullptr;
			float tShadow = std::numeric_limits<float>::max();

			bool vis = !trace(shadowOrig, lightDir, spheres, tShadow, shadowHitSphere);
			diffuseLightInt += vis * light.intensity * std::max(0.f, normalRay.dotProduct(lightDir));
			specularLightInt += vis * std::powf(std::max(0.f, reflect(lightDir, normalRay).dotProduct(-dir)),
				hitSphere->surface.specularExponent) * light.intensity;
		}

		return hitSphere->color * diffuseLightInt * hitSphere->surface.albedo[0] +
			Vec3f(1.f, 1.f, 1.f) * specularLightInt * hitSphere->surface.albedo[1];
	}

	return Vec3f(0.69f, 0.13f, 0.13f);
}

void render(const std::vector<Sphere> &spheres, const std::vector<Light> &lights)
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
			*(pix++) = castRay(Vec3f(0, 0, 0), dir, spheres, lights);
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
	std::vector<Sphere> spheres;

	Vec3f ivory(0.4f, 0.4f, 0.3f);
	Vec3f pinkRubber(0.30f, 0.20f, 0.28f);

	spheres.push_back(Sphere(Vec3f(-3,   0,   -16), 2, ivory, Surface(Vec2f(0.6f, 0.3f), 50.f)));
	spheres.push_back(Sphere(Vec3f(-1., -1.5, -12), 2, pinkRubber, Surface(Vec2f(0.9f, 0.1f), 10.f)));
	spheres.push_back(Sphere(Vec3f(1.5, -0.5, -18), 3, pinkRubber, Surface(Vec2f(0.9f, 0.1f), 10.f)));
	spheres.push_back(Sphere(Vec3f(7,    5,   -18), 4, ivory, Surface(Vec2f(0.6f, 0.3f), 50.f)));

	std::vector<Light> lights;
	lights.push_back(Light(Vec3f(-20, 20,  20), 1.5f));
	lights.push_back(Light(Vec3f( 30, 50, -25), 1.8f));
	lights.push_back(Light(Vec3f( 30, 20,  30), 1.7f));

	render(spheres, lights);

	return 0;
}
