#ifndef RAY_TRACER_GEOMETRY_H
#define RAY_TRACER_GEOMETRY_H

#include <iostream> 
#include <cmath>

inline constexpr double pi {3.14159265358979323846};

template <typename T>
class Vec2 {
public:
	Vec2() : x(T(0)), y(T(0)) {}
	Vec2(T X, T Y) : x(X), y(Y) {}

	Vec2 operator+ (const Vec2 &v) const
	{
		return Vec2(x + v.x, y + v.y);
	}
	Vec2 operator- (const Vec2 &v) const
	{
		return Vec2(x - v.x, y - v.y);
	}
	Vec2 operator- () const
	{
		return Vec2(-x, -y);
	}
	Vec2 operator* (const T &val) const
	{
		return Vec2(x * val, y * val);
	}
	Vec2 operator/ (const T &val) const
	{
		return Vec2(x / val, y / val);
	}
	Vec2& operator*= (const T &val)
	{
		x *= val; y *= val;
		return *this;
	}
	Vec2& operator/= (const T &val)
	{
		x /= val; y /= val;
		return *this;
	}
	T& operator[] (const size_t i)
	{
		return (&x)[i];
	}
	const T& operator[] (const size_t i) const
	{
		return (&x)[i];
	}

	friend Vec2 operator* (const T &val, const Vec2 &v)
	{
		return v * val;
	}
	friend Vec2 operator/ (const T &val, const Vec2 &v)
	{
		return v / val;
	}
	friend std::ostream& operator<< (std::ostream &os, const Vec2 &v)
	{
		return os << '[' << v.x << ", " << v.y << ']';
	}

	T x, y;
};

template <typename T>
class Vec3 {
public:
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T X, T Y, T Z) : x(X), y(Y), z(Z) {}

	Vec3 operator+ (const Vec3 &v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	Vec3 operator- (const Vec3 &v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	Vec3 operator- () const
	{
		return Vec3(-x, -y, -z);
	}
	Vec3 operator* (const T &val) const
	{
		return Vec3(x * val, y * val, z * val);
	}
	Vec3 operator/ (const T &val) const
	{
		return Vec3(x / val, y / val, z / val);
	}
	Vec3& operator*= (const T &val)
	{
		x *= val; y *= val; z *= val;
		return *this;
	}
	Vec3& operator/= (const T &val)
	{
		x /= val; y /= val; z /= val;
		return *this;
	}
	T& operator[] (const size_t i)
	{
		return (&x)[i];
	}
	const T& operator[] (const size_t i) const
	{
		return (&x)[i];
	}

	T dotProduct(const Vec3 &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	Vec3 crossProduct(const Vec3 &v) const
	{
		return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
	T norm() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vec3& normalize()
	{
		T n = norm();
		if (n > 0)
			x /= n, y /= n, z /= n;

		return *this;
	}

	friend Vec3 operator* (const T &val, const Vec3 &v)
	{
		return v * val;
	}
	friend Vec3 operator/ (const T &val, const Vec3 &v)
	{
		return v / val;
	}
	friend std::ostream& operator<< (std::ostream &os, const Vec3 &v)
	{
		return os << '[' << v.x << ", " << v.y << ", " << v.z << ']';
	}

	T x, y, z;
};

typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;

#endif //RAY_TRACER_GEOMETRY_H