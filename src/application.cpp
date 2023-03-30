#include "pch.h"
#include "application.h"
#include "renderer.h"
#include "objects.h"
#include "model.h"
#include <chrono>

#define OBJECT_NAME(i) (("Object " + std::to_string(i)).c_str())
#define LIGHT_NAME(i)  (("Light " + std::to_string(i)).c_str())

Framebuffer::Framebuffer(int width, int height)
    : m_width(width), m_height(height), m_buffer(width * height * 3) {}

void Framebuffer::clear() {
    std::fill(m_buffer.begin(), m_buffer.end(), 0);
}

void Framebuffer::setPixel(int x, int y, const Vec3& color) {
    assert(x >= 0 && x < m_width);
    assert(y >= 0 && y < m_height);
    int index = (y * m_width + x) * 3;
    m_buffer[index]     = static_cast<uint8_t>(std::min(1.0f, color.x) * 255);
    m_buffer[index + 1] = static_cast<uint8_t>(std::min(1.0f, color.y) * 255);
    m_buffer[index + 2] = static_cast<uint8_t>(std::min(1.0f, color.z) * 255);
}

Application::Application(int width, int height)
    : m_framebuffer(width, height), m_rerender(false), m_depth(0) {
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("Cannot init GLFW");

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, "Application", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Cannot create window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Cannot load OpenGL functions");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14, NULL, io.Fonts->GetGlyphRangesCyrillic());
    Init();
}

Application::~Application() noexcept {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::Run() {
    while (!glfwWindowShouldClose(m_window)) {
        if (m_rerender) {
            m_framebuffer.clear();


            auto start = std::chrono::high_resolution_clock::now();

            Render(&m_framebuffer, m_camera, m_scene, m_depth);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << "ns\n";
            m_rerender = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawPixels(m_framebuffer.width(), m_framebuffer.height(),
                     GL_RGB, GL_UNSIGNED_BYTE, m_framebuffer.data());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiUpdate();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Application::Init() {
    float aspect = (float)m_framebuffer.width() / m_framebuffer.height();
    m_camera = Camera(Vec3(0, 4, -7), Vec3(0, 0, 0), 45.f, aspect);
    m_scene.addObject(std::make_shared<Cube>());
    m_scene.addLight(Vec3(0, 5, 0));

    // Test image
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        float x = -2 + rand() % 5;
        float y = -2 + rand() % 5;
        float z = -2 + rand() % 5;
        float r = 0.3;
        m_scene.addObject(std::make_shared<Sphere>(Vec3(x, y, z), r));
        Material& material = m_scene.objectAt(i+1)->getMaterial();
        material.diffuse = Vec3(1, 1, 1);
        material.diffuseAlbedo = 0.4f;
        material.refractAlbedo = 1.0f;
        material.refractive = 1.01f;
        m_scene.objectAt(i+1)->getScale() *= -0.3f;
        m_scene.objectAt(i+1)->getScale() -= -0.1f * i;
    }

    m_depth = 3;
    m_rerender = true;
}

void Application::ImGuiUpdate() {
    ImGuiWindowFlags flags = 0;
    ImGui::SetNextWindowPos(ImVec2(900.0f, 0.0f), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(500, 0), ImGuiCond_Always);
    ImGui::Begin("Настройки", nullptr, flags);
    ImGuiUpdateObjects();
    ImGui::Separator();
    ImGuiUpdateLights();
    ImGui::Separator();
    ImGuiUpdateScene();
    ImGui::End();
}

void Application::ImGuiUpdateScene() {
    static bool showPlane = false;
    if (ImGui::SliderInt("Максимальная глубина", &m_depth, 0, 10)) m_rerender = true;
    if (ImGui::SliderFloat3("Позиция камеры", &m_camera.eye().x, -10, 10)) {
        m_camera.update();
        m_rerender = true;
    }
    if (ImGui::SliderFloat3("Взгляд камеры", &m_camera.lookAt().x, -10, 10)) {
        m_camera.update();
        m_rerender = true;
    }
    if (ImGui::SliderFloat("Угол обзора", &m_camera.fov(), 1, 90)) {
        m_camera.update();
        m_rerender = true;
    }
    if (ImGui::SliderFloat("Фоновое освещение", &m_scene.getAmbient(), 0, 1)) m_rerender = true;
    if (ImGui::Checkbox("Показывать плоскость", &showPlane)) {
        m_scene.showPlane(showPlane);
        m_rerender = true;
    }
}

void Application::ImGuiUpdateObjects() {
    static int objectIdx  = 0;
    std::vector<Scene::ObjectRef>& objects = m_scene.objects();
    if (!objects.empty()) {
        if (ImGui::BeginCombo("Объекты", OBJECT_NAME(objectIdx))) {
            for (int i = 0; i < objects.size(); i++) {
                bool selected = objectIdx == i;
                if (ImGui::Selectable(OBJECT_NAME(i), selected))
                    objectIdx = i;
            }
            ImGui::EndCombo();
        }

        Scene::ObjectRef object = m_scene.objectAt(objectIdx);
        Material& material = object->getMaterial();


        if (ImGui::SliderFloat("Масштабирование", &object->getScale(), -10, 10)) {
            object->update();
            m_rerender = true;
        }
        if (ImGui::SliderFloat("Диффузное отражение", &material.diffuseAlbedo, 0, 1)) m_rerender = true;
        if (ImGui::SliderFloat("Зеркальное отражение", &material.specularAlbedo, 0, 1)) m_rerender = true;
        if (ImGui::SliderFloat("Рефракционное отражение", &material.refractAlbedo, 0, 1)) m_rerender = true;
        if (ImGui::SliderFloat("Блеск", &material.shininess, 0, 1000)) m_rerender = true;
        if (ImGui::SliderFloat("Рефракция", &material.refractive, 1, 5)) m_rerender = true;
        if (ImGui::Button("Удалить объект")) {
            objects.erase(objects.begin() + objectIdx);
            objectIdx = 0;
            m_rerender = true;
        }
        ImGui::SameLine();
    }
    if (ImGui::Button("Добавить пузырек")) {
        m_scene.addObject(std::make_shared<Sphere>());
        objectIdx = objects.size() - 1;
        m_rerender = true;
    }


}

void Application::ImGuiUpdateLights() {
    static int lightIdx = 0;
    std::vector<Vec3>& lights = m_scene.lights();
    if (!lights.empty()) {
        if (ImGui::BeginCombo("Освещение", LIGHT_NAME(lightIdx))) {
            for (int i = 0; i < lights.size(); i++) {
                bool selected = lightIdx == i;
                if (ImGui::Selectable(LIGHT_NAME(i), selected))
                    lightIdx = i;
            }
            ImGui::EndCombo();
        }

        Vec3& light = m_scene.lightAt(lightIdx);
        if (ImGui::SliderFloat3("Позиция источника", &light.x, -15, 15)) m_rerender = true;
        if (ImGui::Button("Удалить источник")) {
            lights.erase(lights.begin() + lightIdx);
            lightIdx = 0;
            m_rerender = true;
        }
        ImGui::SameLine();
    }
    if (ImGui::Button("Добавить источник")) {
        lights.emplace_back(Vec3(0, 0, 0));
        lightIdx = lights.size() - 1;
        m_rerender = true;
    }
}