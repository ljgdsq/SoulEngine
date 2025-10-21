#pragma once
#include "Core/SystemInterface.h"
#include "Renderer.h"
#include <memory>
namespace SoulEngine
{
    class RenderSystem : public SystemInterface
    {
    public:
        RenderSystem() : SystemInterface("RenderSystem") {}
        ~RenderSystem() override = default;

        int GetPriority() const override;

        bool Initialize() override;

        void Update(float dt) override;

        void Shutdown() override;

    protected:
        std::unique_ptr<Renderer> renderer_;

    public:
        Renderer *GetRenderer() const { return renderer_.get(); }
    };
}