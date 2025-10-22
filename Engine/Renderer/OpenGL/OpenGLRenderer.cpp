#if defined(SOULENGINE_ENABLE_OPENGL)

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLRenderer.h"
#include "Log/Logger.h"
#include "Window/Platform/GLFWWindow.h"

namespace
{
    void framebuffer_size_callback(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}

namespace SoulEngine
{
    OpenGLRenderer::OpenGLRenderer() = default;
    OpenGLRenderer::~OpenGLRenderer() = default;


    bool OpenGLRenderer::Initialize(IWindow* window)
    {
        m_window = window;
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Error("Failed to initialize GLAD");
            return false;
        }
        glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
        window->SetFramebufferSizeCallback(framebuffer_size_callback);

        m_initialized = true;
        Logger::Log("OpenGLRenderer initialized successfully");

        return true;
    }

    void OpenGLRenderer::BeginFrame()
    {
        // if (m_window) glfwPollEvents();
    }

    void OpenGLRenderer::EndFrame()
    {
        // if (m_window) glfwSwapBuffers(m_window);
    }

    void OpenGLRenderer::Shutdown()
    {
        Logger::Log("OpenGLRenderer Shutdown");
        m_initialized = false;
    }

    void OpenGLRenderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.2f, 0.1f);
    }

    void OpenGLRenderer::SwapBuffers()
    {
        m_window->SwapBuffers();
    }
} // namespace SoulEngine

#endif
