#pragma once

#include "EngineSystem.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererFactory.h"
#include "Engine.h"
#include <memory>

namespace SoulEngine {

class RendererSystem : public IEngineSystem {
public:
    const char* GetName() const override { return "RendererSystem"; }
    int GetPriority() const override { return 10; } // 较早初始化

    bool Initialize(Engine& engine) override {
        engine_ = &engine;
        renderer_ = RendererFactory::CreateDefault();
        if (!renderer_) return true; // 允许无后端运行
        if (!renderer_->Initialize()) return false;
        engine_->RegisterService<Renderer>(renderer_.get());
        return true;
    }

    void Update(float /*dt*/) override {}

    void Shutdown() override {
        if (engine_ && renderer_) {
            engine_->UnregisterService<Renderer>(renderer_.get());
        }
        if (renderer_) {
            renderer_->Shutdown();
            renderer_.reset();
        }
        engine_ = nullptr;
    }

    Renderer* GetRenderer() const { return renderer_.get(); }

    void BeginFrame() { if (renderer_) renderer_->BeginFrame(); }
    void EndFrame()   { if (renderer_) renderer_->EndFrame(); }

private:
    std::unique_ptr<Renderer> renderer_;
    Engine* engine_ = nullptr;
};

} // namespace SoulEngine
