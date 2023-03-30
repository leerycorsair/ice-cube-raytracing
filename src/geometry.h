#pragma once

#include <cmath>
#include <cassert>
#include <array>
#include <vector>

constexpr float EPSILON = 1e-6;

inline float Sign(float value) noexcept {
    if (value < 0.0f) return -1.0f;
    if (value > 0.0f) return 1.0f;
    return 0.0f;
}

struct Vec3 {
    float x;
    float y;
    float z;

    explicit Vec3(float v = 0.0f) : x(v), y(v), z(v) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
    Vec3(const Vec3& other) = default;

    Vec3& operator=(const Vec3& other) = default;
    float& operator[](int i) noexcept {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }
    float operator[](int i) const noexcept {
        assert(i >= 0 && i <= 2);
        return (&x)[i];
    }
};

inline Vec3& operator+=(Vec3& vector, const Vec3& other) noexcept {
    vector.x += other.x;
    vector.y += other.y;
    vector.z += other.z;
    return vector;
}

inline Vec3& operator+=(Vec3& vector, float value) noexcept {
    vector.x += value;
    vector.y += value;
    vector.z += value;
    return vector;
}

inline Vec3& operator-=(Vec3& vector, const Vec3& other) noexcept {
    vector.x -= other.x;
    vector.y -= other.y;
    vector.z -= other.z;
    return vector;
}

inline Vec3& operator-=(Vec3& vector, float value) noexcept {
    vector.x -= value;
    vector.y -= value;
    vector.z -= value;
    return vector;
}

inline Vec3& operator*=(Vec3& vector, float factor) noexcept {
    vector.x *= factor;
    vector.y *= factor;
    vector.z *= factor;
    return vector;
}

inline Vec3& operator*=(Vec3& vector, const Vec3& other) noexcept {
    vector.x *= other.x;
    vector.y *= other.y;
    vector.z *= other.z;
    return vector;
}

inline Vec3& operator/=(Vec3& vector, float divider) noexcept {
    assert(divider < 0 || divider > 0);
    vector.x /= divider;
    vector.y /= divider;
    vector.z /= divider;
    return vector;
}

inline Vec3 operator-(const Vec3& vector) noexcept {
    return {-vector.x, -vector.y, -vector.z};
}

inline Vec3 operator+(const Vec3& a, const Vec3& b) noexcept {
    Vec3 result = a;
    return result += b;
}

inline Vec3 operator+(const Vec3& vector, float value) noexcept {
    Vec3 result = vector;
    return result += value;
}

inline Vec3 operator-(const Vec3& a, const Vec3& b) noexcept {
    Vec3 result = a;
    return result -= b;
}

inline Vec3 operator-(const Vec3& vector, float value) noexcept {
    Vec3 result = vector;
    return result -= value;
}

inline Vec3 operator*(const Vec3& vector, float factor) noexcept {
    Vec3 result = vector;
    return result *= factor;
}

inline Vec3 operator*(float factor, const Vec3& vector) noexcept {
    Vec3 result = vector;
    return result *= factor;
}

inline Vec3 operator*(const Vec3& vector, const Vec3& other) noexcept {
    Vec3 result = vector;
    return result *= other;
}

inline Vec3 operator/(const Vec3& vector, float divider) noexcept {
    Vec3 result = vector;
    return result /= divider;
}

inline float Dot(const Vec3& a, const Vec3& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 Cross(const Vec3& a, const Vec3& b) noexcept {
    Vec3 product;
    product.x = a.y * b.z - a.z * b.y;
    product.y = a.z * b.x - a.x * b.z;
    product.z = a.x * b.y - a.y * b.x;
    return product;
}

inline float Length(const Vec3& vector) noexcept {
    return std::sqrt(Dot(vector, vector));
}

inline Vec3 Normalize(const Vec3& vector) noexcept {
    return vector / Length(vector);
}

inline Vec3 Reflect(const Vec3& incidence, const Vec3& normal) noexcept {
    return incidence - 2.f * normal * Dot(incidence, normal);
}

inline Vec3 Refract(const Vec3& I, const Vec3& N, const float eta_t, const float eta_i=1.f) {
    float cosi = - std::max(-1.f, std::min(1.f, Dot(I,N)));
    if (cosi<0) return Refract(I, -N, eta_i, eta_t);
    float eta = eta_i / eta_t;
    float k = 1 - eta*eta*(1 - cosi*cosi);
    return k<0 ? Vec3(1,0,0) : I*eta + N*(eta*cosi - std::sqrt(k));
}

class Transform {
public:
    static Transform identity();
    static Transform translate(float translateX, float translateY, float translateZ);
    static Transform rotateX(float angle, const Vec3& center);
    static Transform rotateY(float angle, const Vec3& center);
    static Transform rotateZ(float angle, const Vec3& center);
    static Transform scale(const Vec3& center, float scaleX, float scaleY, float scaleZ);

    Transform() = default;
    Transform(float a00, float a01, float a02, float a03,
              float a10, float a11, float a12, float a13,
              float a20, float a21, float a22, float a23,
              float a30, float a31, float a32, float a33);
    Vec3 apply(const Vec3& vector) const;

private:
    std::array<std::array<float,4>,4> matrix;
};

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& o, const Vec3& d)
        : origin(o), direction(d) {}

    Vec3 at(float t) const noexcept {
        return origin + t * direction;
    }
};

class AABB {
public:
    AABB() = default;
    AABB(const Vec3& minimum, const Vec3& maximum)
        : m_min(minimum), m_max(maximum) {}
    explicit AABB(const std::vector<Vec3>& vertices);

    bool hit(const Ray& ray) const;
    Vec3 min() const { return m_min; }
    Vec3 max() const { return m_max; }

private:
    Vec3 m_min;
    Vec3 m_max;
};
