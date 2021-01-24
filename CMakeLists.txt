cmake_minimum_required(VERSION 3.17)
project(ray-tracer-from-scratch)

set(CMAKE_CXX_STANDARD 17)

add_executable(ray-tracer-from-scratch src/main.cpp src/geometry.h src/Surface.h src/PointLight.h src/Ray.h src/Object.h src/Sphere.cpp src/Sphere.h src/Plane.cpp src/Plane.h src/Settings.h src/Color.h src/Scene.cpp src/Scene.h)