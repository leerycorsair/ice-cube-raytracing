#pragma once

#include <optional>
#include "renderer.h"

class Sphere : public IObject {
public:
    Sphere();
    Sphere(const Vec3& c, float r);
    std::optional<HitRecord> hit(const Ray& ray) const override;
    Material& getMaterial() override { return material; }
    Vec3& getPosition() override { return center; }
    Vec3& getRotation() override { return rotation; }
    float& getScale() override { return radius; }
    void update() override {}

private:
    Vec3     center;
    Vec3     rotation;
    float    radius;
    Material material;
};

class Cube : public IObject {
public:
    Cube();
    Cube(const Vec3& min, const Vec3& max);
    std::optional<HitRecord> hit(const Ray& ray) const override;
    Material& getMaterial() override { return material; }
    Vec3& getPosition() override { return min; }
    Vec3& getRotation() override { return min; }
    float& getScale() override { return radius; }
    void update() override;

private:
    Vec3 min;
    Vec3 max;
    Vec3 center;
    float radius;
    Material material;
};