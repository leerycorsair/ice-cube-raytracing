#include "pch.h"
#include "geometry.h"

constexpr float PI = 3.141592653f;
constexpr float DEG2RAD = PI / 180.f;

Transform::Transform(float a00, float a01, float a02, float a03,
                     float a10, float a11, float a12, float a13,
                     float a20, float a21, float a22, float a23,
                     float a30, float a31, float a32, float a33)
     : matrix({a00, a01, a02, a03,
               a10, a11, a12, a13,
               a20, a21, a22, a23,
               a30, a31, a32, a33}) {}

Transform Transform::identity() {
    return {1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
}

Transform Transform::translate(float translateX, float translateY, float translateZ) {
    return {1.0f, 0.0f, 0.0f, translateX,
            0.0f, 1.0f, 0.0f, translateY,
            0.0f, 0.0f, 1.0f, translateZ,
            0.0f, 0.0f, 0.0f, 1.0f};
}

Transform Transform::rotateX(float angle, const Vec3& center) {
    float rad = angle * DEG2RAD;
    float sin = std::sin(rad);
    float cos = std::cos(rad);
    return {1.0f, 0.0f,  0.0f,  0.0f,
            0.0f,  cos,  -sin, -cos * center.y + sin * center.z + center.y,
            0.0f,  sin,   cos, -cos * center.z - sin * center.y + center.z,
            0.0f, 0.0f,  0.0f,  1.0f};
}

Transform Transform::rotateY(float angle, const Vec3& center) {
    float rad = angle * DEG2RAD;
    float sin = std::sin(rad);
    float cos = std::cos(rad);
    return { cos,  0.0f,  sin, -cos * center.x - sin * center.z + center.x,
             0.0f, 1.0f, 0.0f,  0.0f,
             -sin,  0.f,  cos, -cos * center.z + sin * center.x + center.z,
             0.0f, 0.0f, 0.0f,  1.0f};
}

Transform Transform::rotateZ(float angle, const Vec3& center) {
    float rad = angle * DEG2RAD;
    float sin = std::sin(rad);
    float cos = std::cos(rad);
    return {cos,  -sin, 0.0f, -cos * center.x + sin * center.y + center.x,
            sin,   cos, 0.0f, -cos * center.y - sin * center.x + center.y,
            0.0f, 0.0f, 1.0f,  0.f,
            0.0f, 0.0f, 0.0f,  1.0f};
}

Transform Transform::scale(const Vec3& center, float scaleX, float scaleY, float scaleZ) {
    return {scaleX,  0.0f,    0.0f,    center.x * (1.0f - scaleX),
            0.0f,    scaleY,  0.0f,    center.y * (1.0f - scaleY),
            0.0f,    0.0f,    scaleZ,  center.z * (1.0f - scaleZ),
            0.0f,    0.0f,    0.0f,    1.0f};
}

Vec3 Transform::apply(const Vec3& vector) const {
    Vec3 result;
    result.x = matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z + matrix[0][3];
    result.y = matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * vector.z + matrix[1][3];
    result.z = matrix[2][0] * vector.x + matrix[2][1] * vector.y + matrix[2][2] * vector.z + matrix[2][3];
    float w  = matrix[3][0] * vector.x + matrix[3][1] * vector.y + matrix[3][2] * vector.z + matrix[3][3];
    return result / w;
}

AABB::AABB(const std::vector<Vec3>& vertices) {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& vertex : vertices) {
        minX = std::min(minX, vertex.x);
        minY = std::min(minY, vertex.y);
        minZ = std::min(minZ, vertex.z);
        maxX = std::max(maxX, vertex.x);
        maxY = std::max(maxY, vertex.y);
        maxZ = std::max(maxZ, vertex.z);
    }
    m_min = Vec3(minX, minY, minZ);
    m_max = Vec3(maxX, maxY, maxZ);
}

bool AABB::hit(const Ray& ray) const {
    for (int axis = 0; axis < 3; axis++) {
        float invD = 1.0f / ray.direction[axis];
        float t0 = (m_min[axis] - ray.origin[axis]) * invD;
        float t1 = (m_max[axis] - ray.origin[axis]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        if (t1 <= t0) return false;
    }
    return true;
}
