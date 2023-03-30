#pragma once

#include <cmath>
#include <vector>
#include "geometry.h"

struct Material {
    Material() = default;
    Vec3  diffuse;
    Vec3  specular;
    float diffuseAlbedo = 0.145f;
    float specularAlbedo = 0.125f;
    float reflectAlbedo = 0.0f;
    float refractAlbedo = 0.655f;
    float shininess = 1000.0f;
    float refractive = 4.0f;
};

struct HitRecord {
    Vec3     position;
    Vec3     normal;
    float    parameter;
    Material material;
};

struct IObject {
    virtual std::optional<HitRecord> hit(const Ray& ray) const = 0;
    virtual Material& getMaterial() = 0;
    virtual Vec3& getPosition() = 0;
    virtual Vec3& getRotation() = 0;
    virtual float& getScale() = 0;
    virtual void update() = 0;
};

struct IFramebuffer {
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual void clear() = 0;
    virtual void setPixel(int x, int y, const Vec3& color) = 0;
};

class Camera {
public:
    Camera() = default;
    Camera(const Vec3& eye, const Vec3& lookat, float fov, float aspect);
    Ray generateRay(float s, float t) const;
    void update();

    Vec3&  eye()    { return m_eye; }
    Vec3&  lookAt() { return m_lookAt; }
    float& fov()    { return m_fov; }

private:
    Vec3  m_eye;
    Vec3  m_lookAt;
    float m_fov;
    float m_aspect;
    Vec3  m_horizontal;
    Vec3  m_vertical;
    Vec3  m_corner;
};

class Scene {
public:
    typedef std::shared_ptr<IObject> ObjectRef;

    Scene() = default;
    std::optional<HitRecord> hit(const Ray& ray) const;

    void addObject(const ObjectRef& object)       { m_objects.push_back(object); }
    ObjectRef objectAt(int index)                 { return m_objects[index]; }
    std::vector<ObjectRef>& objects()             { return m_objects; }
    const std::vector<ObjectRef>& objects() const { return m_objects; }

    void addLight(const Vec3& light)        { m_lights.push_back(light); }
    Vec3& lightAt(int index)                { return m_lights[index]; }
    std::vector<Vec3>& lights()             { return m_lights; }
    const std::vector<Vec3>& lights() const { return m_lights; }

    void showPlane(bool show) { m_showPlane = show; }
    float& getAmbient()       { return m_ambient; }
    float  getAmbient() const { return m_ambient; }

private:
    std::vector<ObjectRef> m_objects;
    std::vector<Vec3> m_lights;
    bool  m_showPlane = false;
    float m_ambient = 0.0f;
};

void Render(IFramebuffer* framebuffer, const Camera& camera, const Scene& scene, int depth);
