#pragma once

#include <memory>
#include "Renderer/Renderer.h"

namespace SoulEngine {

class RendererFactory {
public:
    static std::unique_ptr<Renderer> CreateDefault();
};

} // namespace SoulEngine
