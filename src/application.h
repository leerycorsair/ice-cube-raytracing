#pragma once

#include <GLFW/glfw3.h>
#include "renderer.h"

class Framebuffer : public IFramebuffer {
public:
    Framebuffer(int width, int height);
    int width()  const override { return m_width; }
    int height() const override { return m_height; }
    void clear() override;
    void setPixel(int x, int y, const Vec3& color) override;
    const uint8_t* data() const { return m_buffer.data(); }

private:
    int m_width;
    int m_height;
    std::vector<uint8_t> m_buffer;
};

class Application {
public:
    Application(int width, int height);
    Application(const Application& other) = delete;
    Application& operator=(const Application& other) = delete;
    ~Application() noexcept;
    void Run();

private:
    void Init();
    void ImGuiUpdate();
    void ImGuiUpdateScene();
    void ImGuiUpdateObjects();
    void ImGuiUpdateLights();

private:
    GLFWwindow*  m_window;
    Framebuffer  m_framebuffer;
    Camera       m_camera;
    Scene        m_scene;
    int          m_depth;
    bool         m_rerender;
};
