#include "GuiManager.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ContentBrowser.h"
#include "Logger.h"
#include "WindowFactory.h"
#include "Project/ProjectManager.h"

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
        auto window = GetWindow(name);
        if (window == nullptr)
        {
            window =  WindowFactory::GetInstance().CreateWindow(name);
            window->OnInitialize();
            m_windows.push_back(window);
        }
        window->SetVisible(true);
        return window;
    }

    void GuiManager::CloseWindow(std::shared_ptr<GuiWindow> window)
    {
        window->SetVisible(false);
        m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
    }

    void GuiManager::CloseWindow(const std::string& name)
    {
        auto window= GetWindow(name);
        if (window != nullptr)
        {
            CloseWindow(window);
        }
    }

    std::shared_ptr<GuiWindow> GuiManager::GetWindow(const std::string& name) const
    {
        for (auto window : m_windows)
        {
            if (window->GetName() == name)
            {
                return window;
            }
        }
        return nullptr;
    }

    void GuiManager::SwitchToMainEditor()
    {
        CloseWindow("StartupWindow");
        auto browser=OpenWindow<ContentBrowser>("ContentBrowser");
        browser->SetRootPath(ProjectManager::GetInstance().GetCurrentProject()->GetAssetsPath());
    }

    void GuiManager::SwitchToStartupScreen()
    {
        OpenWindow("StartupWindow");
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
