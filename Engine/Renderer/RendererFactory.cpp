#include "RendererFactory.h"
#include <spdlog/spdlog.h>

#if defined(SOULENGINE_ENABLE_OPENGL)
#include "Renderer/OpenGL/OpenGLRenderer.h"
#endif

namespace SoulEngine {

std::unique_ptr<Renderer> RendererFactory::CreateDefault() {
#if defined(SOULENGINE_ENABLE_OPENGL)
    spdlog::info("RendererFactory: creating OpenGL renderer");
    return std::make_unique<OpenGLRenderer>();
#else
    spdlog::warn("RendererFactory: no renderer backend enabled. Returning nullptr");
    return nullptr;
#endif
}

} // namespace SoulEngine
