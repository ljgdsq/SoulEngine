#include "OpenGLRenderer.h"
#include "Log/Logger.h"
// Only compile this file when OpenGL backend is enabled via CMake
#if defined(SOULENGINE_ENABLE_OPENGL)

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#if defined(_WIN32)
#include <Windows.h>
#endif


namespace SoulEngine {

OpenGLRenderer::OpenGLRenderer() = default;
OpenGLRenderer::~OpenGLRenderer() = default;

bool OpenGLRenderer::Initialize() {
//     if (m_initialized) return true;
//     if (!glfwInit()) {
//         Logger::Error("Failed to initialize GLFW");
//         return false;
//     }
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
// #if defined(__APPLE__)
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif
//     m_window = glfwCreateWindow(800, 600, "SoulEngine OpenGL", nullptr, nullptr);
//     if (!m_window) {
//         Logger::Error("Failed to create GLFW window");
//         glfwTerminate();
//         return false;
//     }
//
//     glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
//     
//     glfwMakeContextCurrent(m_window);
//     Logger::Log("GLFW window and context created");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::Error("Failed to initialize GLAD");
        return false;
    }
    
    m_initialized = true;
    return true;
}

void OpenGLRenderer::BeginFrame() {
    // if (m_window) glfwPollEvents();
}

void OpenGLRenderer::EndFrame() {
    // if (m_window) glfwSwapBuffers(m_window);
}

void OpenGLRenderer::Shutdown() {
    // if (m_window) {
    //     glfwDestroyWindow(m_window);
    //     m_window = nullptr;
    // }
    glfwTerminate();
    m_initialized = false;
}

void OpenGLRenderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f,0.1f,0.2f,0.1f);
}

} // namespace SoulEngine

#else

// If OpenGL is not enabled, provide empty file to avoid build issues
namespace SoulEngine { /* no-op OpenGLRenderer without backend enabled */ }

#endif
