#pragma once
#include <imgui.h>
#include <string>
#include "WindowRegister.h"
namespace SoulEditor
{
    class GuiWindow
    {
    public:
        GuiWindow(const std::string& name) : m_name(name)
        {
        }

        virtual ~GuiWindow() = default;

    public:
        // 核心接口
        virtual void OnInitialize()
        {
        }

        virtual void OnUpdate(float deltaTime)
        {
        }

        virtual void OnRender()
        {
        }

        virtual void OnShutdown()
        {
        }

    protected:
        virtual bool CanClose() const { return true; }

    public:
        const std::string& GetName() const { return m_name; }
        bool IsVisible() const { return m_isVisible; }
        void SetVisible(bool visible) { m_isVisible = visible; }

    protected:
        std::string m_name;
        bool m_isVisible = true;
        // ImVec2 m_size = ImVec2(400, 300);
        // ImVec2 m_position = ImVec2(100, 100);
    };
}
