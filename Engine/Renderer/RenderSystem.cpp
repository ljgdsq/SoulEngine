#include "RenderSystem.h"
#include "Log/Logger.h"
#if defined(SOULENGINE_ENABLE_OPENGL)
#include "Renderer/OpenGL/OpenGLRenderer.h"
#endif

namespace SoulEngine
{

    int RenderSystem::GetPriority() const
    {
        return 10; // 较早初始化
    }

    bool RenderSystem::Initialize()
    {
        Logger::Log("Initializing RenderSystem...");
#if defined(SOULENGINE_ENABLE_OPENGL)
        Logger::Log("RendererFactory: creating OpenGL renderer");
        renderer_ = std::make_unique<OpenGLRenderer>();
#else
        Logger::Warn("RendererFactory: no renderer backend enabled. Returning nullptr");
#endif
        return true;
    }

    void RenderSystem::Update(float dt)
    {
    }

    void RenderSystem::Shutdown()
    {
        Logger::Log("Shutting down RenderSystem...");
        if (renderer_)
        {
            renderer_->Shutdown();
            renderer_.reset();
        }
    }
} // namespace SoulEngine