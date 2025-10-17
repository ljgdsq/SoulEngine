#pragma once

#include "Renderer/Renderer.h"

struct GLFWwindow;

namespace SoulEngine {

class OpenGLRenderer final : public Renderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer() override;

    bool Initialize() override;
    void BeginFrame() override;
    void EndFrame() override;
    void Shutdown() override;
    void Clear() override;

private:
    GLFWwindow* m_window = nullptr;
    bool m_initialized = false;
};

} // namespace SoulEngine
