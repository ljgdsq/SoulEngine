#if defined(SOULENGINE_ENABLE_OPENGL)

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLRenderer.h"
#include "Log/Logger.h"
#include "Window/Platform/GLFWWindow.h"
#include "Renderer/Gfx.h"
#include "Renderer/OpenGL/GfxGLDevice.h"
#include "Renderer/OpenGL/GfxGLContext.h"

using namespace SoulEngine::Gfx;
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

        // Create Gfx device/context now that GL is initialized
        device_ = std::make_shared<GfxGLDevice>();
        context_ = std::make_shared<GfxGLContext>();

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
        context_.reset();
        device_.reset();
        m_initialized = false;
    }

    void OpenGLRenderer::Clear()
    {
        glClearColor(0.1f, 0.1f, 0.2f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::SwapBuffers()
    {
        m_window->SwapBuffers();
    }

    Gfx::IDevice* OpenGLRenderer::GetGfxDevice()
    {
        return device_.get();
    }

    Gfx::IContext* OpenGLRenderer::GetGfxContext()
    {
        return context_.get();
    }
} // namespace SoulEngine

#endif
