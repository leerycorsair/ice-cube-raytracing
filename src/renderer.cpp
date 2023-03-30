#include "pch.h"
#include "renderer.h"
#include "geometry.h"

Camera::Camera(const Vec3 &eye, const Vec3 &lookat, float fov, float aspect)
{
    m_eye = eye;
    m_lookAt = lookat;
    m_fov = fov;
    m_aspect = aspect;
    update();
}

Ray Camera::generateRay(float s, float t) const
{
    Vec3 point = m_corner + s * m_horizontal + t * m_vertical;
    return {m_eye, point - m_eye};
}

void Camera::update()
{
    Vec3 n = Normalize(m_eye - m_lookAt);
    Vec3 u = Normalize(Cross(Vec3(0.0f, 1.0f, 0.0f), n));
    Vec3 v = Cross(n, u);

    float h = std::tan(m_fov * M_PI / 360.0f);
    float viewportHeight = 2.f * h;
    float viewportWidth = viewportHeight * m_aspect;

    m_horizontal = u * viewportWidth;
    m_vertical = v * viewportHeight;
    m_corner = m_eye - m_horizontal / 2.f - m_vertical / 2.f - n;
}

std::optional<HitRecord> Scene::hit(const Ray &ray) const
{
    std::optional<HitRecord> result = std::nullopt;
    float minT = std::numeric_limits<float>::max();

    if (m_showPlane && std::abs(ray.direction.y) > 0.001f)
    {
        float t = -(ray.origin.y + 3.0f) / ray.direction.y;
        Vec3 p = ray.origin + ray.direction * t;
        if (t > 0.001f && t < minT)
        {
            HitRecord record;
            record.position = p;
            record.normal = Vec3(0.0f, 1.0f, 0.0f);
            record.parameter = t;
            record.material.diffuseAlbedo = 1.0f;
            record.material.diffuse = Vec3(0.8, 0.8, 0.8);
            result = record;
            minT = t;
        }
    }

    for (const auto &object : m_objects)
    {
        auto record = object->hit(ray);
        if (record && record->parameter < minT)
        {
            minT = record->parameter;
            result = record;
        }
    }
    return result;
}

static Vec3 background(const Ray &ray)
{
    Vec3 direction = Normalize(ray.direction);
    float t = 0.5f * (direction.y + 1.f);
    return (1.f - t) * Vec3(1.f, 1.f, 1.f) + t * Vec3(0.5f, 0.7f, 1.f);
}

static Vec3 castRay(const Ray &ray, const Scene &scene, int depth)
{
    if (depth <= 0)
        return std::min(1.0f, 2 * scene.getAmbient()) * background(ray);

    if (std::optional<HitRecord> record = scene.hit(ray))
    {
        Material &material = record->material;
        Vec3 reflectDir = Reflect(ray.direction, record->normal);
        Vec3 refractDir = Refract(ray.direction, record->normal, material.refractive);
        Vec3 reflected = castRay(Ray(record->position, reflectDir), scene, depth - 1);
        Vec3 refracted = castRay(Ray(record->position, refractDir), scene, depth - 1);

        float diffuse = scene.getAmbient();
        float specular = 0.0f;
        for (const Vec3 &light : scene.lights())
        {
            Vec3 source = Normalize(light - record->position);
            diffuse += std::max(0.0f, Dot(source, record->normal));
            Vec3 r = -Reflect(-source, record->normal);
            specular += std::pow(std::max(0.0f, Dot(r, Normalize(ray.direction))), material.shininess);
        }
        return material.diffuseAlbedo * material.diffuse * std::min(1.0f, diffuse) +
               material.specularAlbedo * material.specular * std::min(1.0f, specular) +
               material.reflectAlbedo * reflected +
               material.refractAlbedo * refracted;
    }
    return std::min(1.0f, 2 * scene.getAmbient()) * background(ray);
}

void Render(IFramebuffer *framebuffer, const Camera &camera, const Scene &scene, int depth)
{
    int width = framebuffer->width();
    int height = framebuffer->height();

#pragma omp parallel for num_threads(12)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float s = (float)x / (width - 1);
            float t = (float)y / (height - 1);

            Ray ray = camera.generateRay(s, t);
            framebuffer->setPixel(x, y, castRay(ray, scene, depth));
        }
    }
}
