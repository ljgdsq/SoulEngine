#pragma once

#include "Renderer/Renderer.h"


namespace SoulEngine
{
    class IWindow;

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

    private:
        bool m_initialized = false;
        IWindow* m_window = nullptr;
    };
} // namespace SoulEngine
