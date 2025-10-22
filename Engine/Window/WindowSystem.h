#pragma once
#include "Core/SystemInterface.h"
#include "Window/IWindow.h"

namespace SoulEngine
{
    class WindowSystem : public SystemInterface
    {
    public:
        WindowSystem() : SystemInterface("WindowSystem")
        {
        }

        ~WindowSystem() override = default;

        int GetPriority() const override;

        bool Initialize() override;

        void Update(float dt) override;

        void Shutdown() override;

    private:
        IWindow* m_window = nullptr;

    public:
        IWindow* GetWindow() const { return m_window; }
    };
} // namespace SoulEngine
