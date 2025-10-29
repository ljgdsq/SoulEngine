#include "GuiManager.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Logger.h"
#include "WindowFactory.h"

namespace SoulEditor
{
    
    bool GuiManager::Initialize(GLFWwindow* window)
    {

        if (initialized)
            return false;
    
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // 配置ImGui
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 启用手柄控制
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // 启用多视口/平台窗口
        

        // 设置平台/渲染器后端
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        SetupStyle();
        ImGui_ImplOpenGL3_Init("#version 330 core");
        initialized = true;
        SoulEngine::Logger::Log("GuiManager initialized successfully");

        OpenWindow("StartupWindow");
        
        return true;
    }

    void GuiManager::BeginFrame()
    {
        if (!initialized)
            return;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiManager::EndFrame()
    {
        if (!initialized)
            return;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // 更新和渲染额外的平台窗口
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void GuiManager::Render() const
    {
        for (auto window : m_windows)
        {
            if (window->IsVisible())
            {
                window->OnRender();
            }
        }
    }

    void GuiManager::SetupStyle()
    {
        ImGui::StyleColorsDark();
        
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();
        
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        // 可以在这里添加更多的样式自定义
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 3.0f;
    }

    std::shared_ptr<GuiWindow> GuiManager::OpenWindow(std::string name)
    {
        auto window =  WindowFactory::GetInstance().CreateWindow(name);
        m_windows.push_back(window);
        return window;
    }

    void GuiManager::Shutdown()
    {
        if (!initialized)
            return;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        initialized = false;
        SoulEngine::Logger::Log("GuiManager shutdown");
    }

}
