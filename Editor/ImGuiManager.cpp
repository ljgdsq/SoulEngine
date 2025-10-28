#include "ImGuiManager.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "SoulEngine.h"

namespace SoulEditor
{
    bool ImGuiManager::Initialize(GLFWwindow* window)
    {
        if (isInitialized_)
            return true;
            
        // 创建ImGui上下文
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        
        // 配置ImGui
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 启用手柄控制
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // 启用多视口/平台窗口
        
        // 注意：不在初始化时启用Docking，而是动态控制
        
        // 设置样式
        SetupStyle();
        
        // 设置平台/渲染器后端
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        
        // 使用适当的GLSL版本
#ifdef __APPLE__
        const char* glsl_version = "#version 330 core";  // macOS with OpenGL 3.3 Core Profile
#else
        const char* glsl_version = "#version 130";        // 其他平台
#endif
        
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        isInitialized_ = true;
        SoulEngine::Logger::Log("ImGui initialized successfully");
        
        return true;
    }
    
    void ImGuiManager::Shutdown()
    {
        if (!isInitialized_)
            return;
            
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        isInitialized_ = false;
        SoulEngine::Logger::Log("ImGui shutdown");
    }
    
    void ImGuiManager::BeginFrame()
    {
        if (!isInitialized_)
            return;
            
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    
    void ImGuiManager::EndFrame()
    {
        if (!isInitialized_)
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
    
    void ImGuiManager::EnableDocking(bool enable)
    {
        ImGuiIO& io = ImGui::GetIO();
        
        if (enable && !isDockingEnabled_)
        {
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            isDockingEnabled_ = true;
            SoulEngine::Logger::Log("Docking enabled");
        }
        else if (!enable && isDockingEnabled_)
        {
            io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
            isDockingEnabled_ = false;
            SoulEngine::Logger::Log("Docking disabled");
        }
    }
    
    void ImGuiManager::CreateDockSpace()
    {
        if (!isDockingEnabled_)
            return;
            
        static ImGuiID dockspaceId = ImGui::GetID("EditorPanelDockSpace");
        ImGui::DockSpaceOverViewport(dockspaceId, ImGui::GetMainViewport());
    }
    
    void ImGuiManager::SetupStyle()
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
}