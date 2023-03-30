#pragma once

#include <vector>
#include <string>
#include <optional>
#include "geometry.h"
#include "renderer.h"

class Model : public IObject {
public:
    explicit Model(const std::string& filename);
    std::optional<HitRecord> hit(const Ray& ray) const override;
    Material& getMaterial() override { return m_material; }
    Vec3& getPosition() override { return m_center; }
    Vec3& getRotation() override { return m_rotation; }
    float& getScale() override { return m_scale; }
    void update() override;

private:
    Vec3 vertex(int face, int vertex) const;

private:
    std::vector<Vec3> m_vertices;
    std::vector<int>  m_faces;
    AABB              m_aabb;
    Material m_material;
    Vec3     m_center;
    Vec3     m_rotation;
    float    m_scale;
};
