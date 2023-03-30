#include "pch.h"
#include "model.h"

static std::optional<HitRecord> intersection(const Vec3& A, const Vec3& B, const Vec3& C, const Ray &ray);

Model::Model(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open())
        throw std::runtime_error("Cannot open model");

    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            Vec3 v;
            iss >> trash >> v.x >> v.y >> v.z;
            m_vertices.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            int vertex;
            int count = 0;
            while (iss >> vertex) {
                m_faces.push_back(vertex - 1);
                count++;
            }
            if (count != 3) throw std::runtime_error("Not triangulated model");
        }
    }

    m_aabb = AABB(m_vertices);
    m_center = (m_aabb.min() + m_aabb.max()) / 2.0f;
}

std::optional<HitRecord> Model::hit(const Ray& ray) const {
    if (!m_aabb.hit(ray))
        return std::nullopt;

    std::optional<HitRecord> hitRecord = std::nullopt;
    float minT = std::numeric_limits<float>::max();

    for (int face = 0; face < m_faces.size() / 3; face++) {
        Vec3 A = vertex(face, 0);
        Vec3 B = vertex(face, 1);
        Vec3 C = vertex(face, 2);
        auto record = intersection(A, B, C, ray);
        if (record && record->parameter < minT) {
            hitRecord = record;
            hitRecord->material = m_material;
            minT = record->parameter;
        }
    }
    return hitRecord;
}

void Model::update() {

}

Vec3 Model::vertex(int face, int vertex) const {
    assert(face >= 0 && face < m_faces.size() / 3);
    assert(vertex >= 0 && vertex < 3);
    int index = m_faces[face * 3 + vertex];
    return m_vertices[index];
}

static std::optional<HitRecord> intersection(const Vec3& A, const Vec3& B, const Vec3& C, const Ray &ray) {
    Vec3 edge1 = B - A;
    Vec3 edge2 = C - A;
    Vec3 pvec = Cross(ray.direction, edge2);

    float det = Dot(edge1, pvec);
    if (det < EPSILON) return std::nullopt;

    Vec3 tvec = ray.origin - A;
    float u = Dot(tvec, pvec);
    if (u < 0 || u > det) return std::nullopt;

    Vec3 qvec = Cross(tvec, edge1);
    float v = Dot(ray.direction, qvec);
    if (v < 0 || u + v > det) return std::nullopt;

    float t = Dot(edge2, qvec) * (1.0f / det);
    if (t < EPSILON) return std::nullopt;

    HitRecord record;
    record.position = ray.at(t);
    record.normal = Normalize(Cross(edge1, edge2));
    record.parameter = t;
    return record;
}
