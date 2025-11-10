#pragma once

#include "Renderer/Renderer.h"
#include <glad/glad.h>
#include <memory>
#include <deque>


namespace SoulEngine
{
    class IWindow;
    namespace Gfx { class IDevice; class IContext; }

    class OpenGLRenderer final : public Renderer
    {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer() override;

        bool Initialize(IWindow* window) override;
        void BeginFrame() override;
        void EndFrame() override;
        void Shutdown() override;
        void Clear() override;
        void SwapBuffers() override;

        // Gfx accessors
        Gfx::IDevice* GetGfxDevice() override;
        Gfx::IContext* GetGfxContext() override;

    private:
        bool m_initialized = false;
        IWindow* m_window = nullptr;

        std::shared_ptr<Gfx::IDevice> device_;
        std::shared_ptr<Gfx::IContext> context_;

        std::deque<GLsync> frameFences_;
        bool forceSync_ = false;
    };
} // namespace SoulEngine
