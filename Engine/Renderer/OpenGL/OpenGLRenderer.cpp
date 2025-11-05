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

    void Renderer::SetMaxFramesInFlight(int maxFrames)
    {
        maxFramesInFlight = std::clamp(maxFrames, 1, 9);
    }

    void OpenGLRenderer::BeginFrame()
    {
        while (!frameFences_.empty())
        {
            GLenum res = glClientWaitSync(frameFences_.front(), 0, 0);
            if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED)
            {
                glDeleteSync(frameFences_.front());
                frameFences_.pop_front();
            }
            else
                break;
        }
        
        forceSync_ = !flushGpu && (frameFences_.size() >= maxFramesInFlight * 2);
        if (forceSync_)
        {
            Logger::Warn("forceSync enabled - GPU is lagging behind CPU by more than {} frames", frameFences_.size());
        }
    }

    void OpenGLRenderer::EndFrame()
    {
        
    }

    void OpenGLRenderer::Shutdown()
    {
        Logger::Log("OpenGLRenderer Shutdown");
        context_.reset();
        device_.reset();
        m_initialized = false;
        while (!frameFences_.empty())
        {
            glDeleteSync(frameFences_.front());
            frameFences_.pop_front();
        }
    }

    void OpenGLRenderer::Clear()
    {
        glClearColor(0.1f, 0.1f, 0.2f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::SwapBuffers()
    {
        m_window->SwapBuffers();
        // optional: flush to ensure commands submitted so fence will observe them
        glFlush();
        
        // create fence and check
        if (GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0))
            frameFences_.push_back(fence);
        else
        {
            Logger::Warn("glFenceSync failed");
        }
        
        while (!frameFences_.empty())
        {
            GLenum res = glClientWaitSync(frameFences_.front(), 0, 0);
            if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED)
            {
                glDeleteSync(frameFences_.front());
                frameFences_.pop_front();
            }
            else
                break;
        }

        if (flushGpu)
        {
            while (frameFences_.size() > maxFramesInFlight)
            {
                GLsync oldFence = frameFences_.front();
                GLenum waitRes = glClientWaitSync(oldFence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
                if (waitRes == GL_WAIT_FAILED)
                {
                    Logger::Warn("glClientWaitSync failed in flushGpu branch");
                    break;
                }
                glDeleteSync(oldFence);
                frameFences_.pop_front();
            }
        }
        else if (forceSync_)
        {
            while (frameFences_.size() > 1)
            {
                GLsync oldFence = frameFences_.front();
                GLenum waitRes = glClientWaitSync(oldFence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
                if (waitRes == GL_WAIT_FAILED)
                {
                    Logger::Warn("glClientWaitSync failed in forceSync branch");
                    break;
                }
                glDeleteSync(oldFence);
                frameFences_.pop_front();
            }
        }
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
