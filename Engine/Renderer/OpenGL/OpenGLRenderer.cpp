#include "OpenGLRenderer.h"
#include <spdlog/spdlog.h>

// Only compile this file when OpenGL backend is enabled via CMake
#if defined(SOULENGINE_ENABLE_OPENGL)

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace SoulEngine {

OpenGLRenderer::OpenGLRenderer() = default;
OpenGLRenderer::~OpenGLRenderer() = default;

bool OpenGLRenderer::Initialize() {
    if (m_initialized) return true;
    if (!glfwInit()) {
        spdlog::error("GLFW init failed");
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_window = glfwCreateWindow(800, 600, "SoulEngine OpenGL", nullptr, nullptr);
    if (!m_window) {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    // Load OpenGL functions via glad using GLFW's loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::error("Failed to load OpenGL functions via glad");
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();
        return false;
    }
    spdlog::info("OpenGL context created (via GLFW)");
    m_initialized = true;
    return true;
}

void OpenGLRenderer::BeginFrame() {
    // In a real implementation, poll events etc.
    if (m_window) glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void OpenGLRenderer::EndFrame() {
    if (m_window) glfwSwapBuffers(m_window);
}

void OpenGLRenderer::Shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
    m_initialized = false;
}

void OpenGLRenderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace SoulEngine

#else

// If OpenGL is not enabled, provide empty file to avoid build issues
namespace SoulEngine { /* no-op OpenGLRenderer without backend enabled */ }

#endif
