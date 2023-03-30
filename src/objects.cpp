#include "pch.h"
#include "objects.h"

Sphere::Sphere() : Sphere(Vec3(0, 0, 0), 1) {}

Sphere::Sphere(const Vec3 &c, float r) : center(c), radius(r) {}

std::optional<HitRecord> Sphere::hit(const Ray &ray) const
{
    Vec3 oc = ray.origin - center;
    float a = Dot(ray.direction, ray.direction);
    float h = Dot(oc, ray.direction);
    float c = Dot(oc, oc) - radius * radius;

    float discriminant = h * h - a * c;
    if (discriminant < 0)
        return std::nullopt;

    float root = (-h - std::sqrt(discriminant)) / a;
    if (root < 0.001f)
        return std::nullopt;

    HitRecord record;
    record.position = ray.at(root);
    record.normal = (record.position - center) / radius;
    record.parameter = root;
    record.material = material;
    return record;
}

Cube::Cube() : Cube(Vec3(-3.0f, -3.0f, -3.0f), Vec3(3.0f, 3.0f, 3.0f)) {}

Cube::Cube(const Vec3 &min, const Vec3 &max) : min(min), max(max), center((min + max) / 2.0f) {}

std::optional<HitRecord> Cube::hit(const Ray &ray) const
{
    int normalAxis = 0;
    float tMin = std::numeric_limits<float>::min();
    float tMax = std::numeric_limits<float>::max();
    for (int axis = 0; axis < 3; axis++)
    {
        float invD = 1.0f / ray.direction[axis];
        float t0 = (min[axis] - ray.origin[axis]) * invD;
        float t1 = (max[axis] - ray.origin[axis]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        if (t0 > tMin)
        {
            tMin = t0;
            normalAxis = axis;
        }
        tMax = std::min(tMax, t1);
    }
    if (tMin >= tMax)
        return std::nullopt;
    if (tMin < 0.001f)
        return std::nullopt;

    Vec3 point = ray.at(tMin);
    Vec3 normal;
    normal[normalAxis] = Sign((point - center)[normalAxis]);

    HitRecord record;
    record.parameter = tMin;
    record.position = point;
    record.normal = normal;
    record.material = material;
    return record;
}

void Cube::update()
{
}