#include "SoulEngine.h"
#include "Core/ApplicationHelper.h"
#include "ProjectManager.h"
#include "WelcomeUI.h"
#include "MainEditorUI.h"
#include "ImGuiManager.h"
#include "Window/WindowSystem.h"

using namespace SoulEngine;
using namespace SoulEngine::Gfx;
using namespace SoulEditor;

/**
 * @brief SoulEditor 主应用程序类
 */
class SoulEditorApplication : public SoulEngine::ApplicationBase
{
public:
    SoulEditorApplication() : ApplicationBase("SoulEditor")
    {
    }

private:
    // UI管理器
    std::unique_ptr<ImGuiManager> imguiManager_;
    std::unique_ptr<WelcomeUI> welcomeUI_;
    std::unique_ptr<MainEditorUI> mainEditorUI_;
    
    // 渲染资源（保留原有的渲染相关变量）
    std::shared_ptr<IProgram> program_;
    std::shared_ptr<IVertexInputLayout> layout_;
    std::shared_ptr<IBuffer> vertexBuffer1_;
    std::shared_ptr<IBuffer> vertexBuffer2_;
    std::shared_ptr<IBuffer> indexBuffer_;

public:
    bool OnInitialize() override
    {
        Logger::Log("SoulEditor initialized");
        
        // 设置窗口不可resize（启动时）
        WindowSystem::SetWindowResizable(GetWindow()->GetGLFWWindow(), false);
        
        // 初始化ImGui管理器
        imguiManager_ = std::make_unique<ImGuiManager>();
        if (!imguiManager_->Initialize(GetWindow()->GetGLFWWindow()))
        {
            Logger::Error("Failed to initialize ImGui manager");
            return false;
        }
        
        // 初始化UI管理器
        welcomeUI_ = std::make_unique<WelcomeUI>();
        mainEditorUI_ = std::make_unique<MainEditorUI>();
        
        // 设置项目管理器回调
        auto& projectManager = ProjectManager::GetInstance();
        projectManager.SetOnProjectOpenedCallback([this](const std::string& projectPath) {
            OnProjectOpened(projectPath);
        });
        projectManager.SetOnProjectClosedCallback([this]() {
            OnProjectClosed();
        });
        
        // 设置UI回调
        welcomeUI_->SetOnCreateProjectCallback([this]() {
            ImGuiFileDialog::Instance()->OpenDialog("CreateProjectDlg", "Select Project Folder", ".");
        });
        welcomeUI_->SetOnOpenProjectCallback([this]() {
            ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", "project.json,.*");
        });
        
        mainEditorUI_->SetOnCloseProjectCallback([this]() {
            ProjectManager::GetInstance().CloseProject();
        });
        
        // 加载最近项目
        projectManager.LoadRecentProjects();
        
        return true;
    }

    void Update(float deltaTime) override
    {
        // 更新逻辑
    }
    
    void Render() override
    {
        // 开始ImGui帧
        imguiManager_->BeginFrame();
        
        // 根据项目状态动态控制Docking
        bool projectOpened = ProjectManager::GetInstance().IsProjectOpened();
        if (projectOpened && !imguiManager_->IsDockingEnabled())
        {
            imguiManager_->EnableDocking(true);
        }
        else if (!projectOpened && imguiManager_->IsDockingEnabled())
        {
            imguiManager_->EnableDocking(false);
        }
        
        // 创建DockSpace（如果需要）
        if (projectOpened)
        {
            imguiManager_->CreateDockSpace();
        }
        
        // 渲染UI
        if (projectOpened)
        {
            mainEditorUI_->Render();
        }
        else
        {
            welcomeUI_->Render();
        }
        
        // 结束ImGui帧
        imguiManager_->EndFrame();
    }

    void Shutdown() override
    {
        // 保存最近项目
        ProjectManager::GetInstance().SaveRecentProjects();
        
        // 清理UI管理器
        mainEditorUI_.reset();
        welcomeUI_.reset();
        
        // 清理ImGui管理器
        if (imguiManager_)
        {
            imguiManager_->Shutdown();
            imguiManager_.reset();
        }
        
        Logger::Log("SoulEditor shutdown");
    }

    bool ShouldClose() const override
    {
        return GetWindow()->ShouldClose();
    }
    
private:
    void OnProjectOpened(const std::string& projectPath)
    {
        Logger::Log("Project opened: {}", projectPath);
        
        // 启用窗口resize功能
        WindowSystem::SetWindowResizable(GetWindow()->GetGLFWWindow(), true);
        
        // 可以在这里添加项目打开后的初始化逻辑
        // 比如加载项目配置、初始化资源管理器等
    }
    
    void OnProjectClosed()
    {
        Logger::Log("Project closed, returning to welcome screen");
        
        // 禁用窗口resize功能
        WindowSystem::SetWindowResizable(GetWindow()->GetGLFWWindow(), false);
        
        // 可以在这里添加项目关闭后的清理逻辑
    }
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(SoulEditorApplication)
    
    // 添加渐变背景
    ImU32 bg_color_top = IM_COL32(40, 44, 52, 255);    // 深灰蓝色
    ImU32 bg_color_bottom = IM_COL32(25, 28, 34, 255); // 更深的灰色
    draw_list->AddRectFilledMultiColor(window_pos, 
                                      ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y),
                                      bg_color_top, bg_color_top, bg_color_bottom, bg_color_bottom);
    
    // 添加一些顶部间距和居中的标题
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50.0f);
    
    // 居中显示标题，使用更大的字体
    float windowWidth = ImGui::GetWindowSize().x;
    const char* title = "Welcome to SoulEditor";
    
    // 使用更大的字体渲染标题
    ImGui::PushFont(ImGui::GetIO().FontDefault); // 可以替换为大字体
    float titleWidth = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
    
    // 使用渐变颜色的标题
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "%s", title);
    ImGui::PopFont();
    
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30.0f);
    
    // 居中显示内容区域
    float contentWidth = 500.0f;
    ImGui::SetCursorPosX((windowWidth - contentWidth) * 0.5f);
    
    ImGui::BeginChild("WelcomeContent", ImVec2(contentWidth, 0), false, ImGuiWindowFlags_NoScrollbar);

    // 设置一个更柔和的提示文本颜色
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.9f, 1.0f));
    ImGui::TextWrapped("Welcome * choose an action:");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // 设置按钮样式
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.8f));         // 蓝色按钮
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));  // 悬停时更亮
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.53f, 0.98f, 1.0f));   // 点击时更深
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);                             // 圆角按钮
    
    if (ImGui::Button("Create Project", ImVec2(200, 40))) {
        // open folder picker (select a directory to create project in)
        ImGuiFileDialog::Instance()->OpenDialog("CreateProjectDlg", "Select Project Folder", ".");
    }

    ImGui::SameLine();
    if (ImGui::Button("Open Project", ImVec2(200, 40))) {
        // open file dialog to choose project.json or project folder
        ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", "project.json,.*");
    }
    
    ImGui::PopStyleVar();      // 弹出圆角设置
    ImGui::PopStyleColor(3);   // 弹出3个颜色设置

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // 最近项目标题
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("Recent Projects:");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // 最近项目列表，使用不同的按钮样式
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    
    for (size_t i = 0; i < recentProjects.size(); ++i) {
        const auto &p = recentProjects[i];
        ImGui::PushID((int)i);
        
        // 只显示项目文件夹名称，而不是完整路径
        std::filesystem::path projectPath(p);
        std::string displayName = projectPath.filename().string();
        if (displayName.empty()) displayName = p; // 如果无法获取文件夹名，显示完整路径
        
        if (ImGui::Button(displayName.c_str(), ImVec2(contentWidth - 20, 0))) { 
            OpenProjectAt(p); 
        }
        
        // 在悬停时显示完整路径
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", p.c_str());
        }
        
        ImGui::PopID();
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    // Handle CreateProject dialog result - 限制对话框的移动和缩放，并居中显示
    // 设置对话框位置居中
    // ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 center = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, 
                          viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
    ImVec2 dialogSize = ImVec2(800, 600);
    ImGui::SetNextWindowPos(ImVec2(center.x - dialogSize.x * 0.5f, center.y - dialogSize.y * 0.5f), ImGuiCond_Appearing);
    
    if (ImGuiFileDialog::Instance()->Display("CreateProjectDlg", 
                                            ImGuiWindowFlags_NoResize | 
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse,
                                            dialogSize, // 固定大小
                                            dialogSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string selPath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // If a file was selected, use its parent directory
            if (!ImGuiFileDialog::Instance()->GetFilePathName().empty()) {
                selPath = std::filesystem::path(ImGuiFileDialog::Instance()->GetFilePathName()).parent_path().string();
            }
            if (!selPath.empty()) CreateProjectAt(selPath);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Handle OpenProject dialog result - 限制对话框的移动和缩放，并居中显示
    // 设置对话框位置居中（复用上面的viewport和center计算）
    ImGui::SetNextWindowPos(ImVec2(center.x - dialogSize.x * 0.5f, center.y - dialogSize.y * 0.5f), ImGuiCond_Appearing);
    
    if (ImGuiFileDialog::Instance()->Display("OpenProjectDlg", 
                                            ImGuiWindowFlags_NoResize | 
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse,
                                            dialogSize, // 固定大小
                                            dialogSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string sel = ImGuiFileDialog::Instance()->GetFilePathName();
            if (sel.empty()) sel = ImGuiFileDialog::Instance()->GetCurrentPath();
            if (!sel.empty()) OpenProjectAt(sel);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::EndChild(); // 结束 WelcomeContent 子窗口
    ImGui::End();
}

// 编辑器面板显示状态
static bool showProjectBrowser = true;
static bool showSceneHierarchy = true;
static bool showInspector = true;
static bool showConsole = true;

void ShowMainEditorUI() {
    // 主编辑器界面
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project")) {
                // 关闭当前项目，显示欢迎界面
                isProjectOpened = false;
                currentProjectRoot.clear();
            }
            if (ImGui::MenuItem("Open Project")) {
                ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", "project.json,.*");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // 设置应用退出标志
                glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Project Browser", nullptr, &showProjectBrowser);
            ImGui::MenuItem("Scene Hierarchy", nullptr, &showSceneHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &showInspector);
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout")) {
                // 重置布局的功能
                showProjectBrowser = true;
                showSceneHierarchy = true;
                showInspector = true;
                showConsole = true;
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // 显示关于对话框
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // 示例编辑器面板 - 可以根据需要添加 ImGuiWindowFlags_NoMove 等标志
    if (showProjectBrowser) {
        ImGui::Begin("Project Browser", &showProjectBrowser);
        ImGui::Text("Current Project: %s", currentProjectRoot.c_str());
        
        // 添加关闭项目按钮
        if (ImGui::Button("Close Project")) {
            isProjectOpened = false;
            currentProjectRoot.clear();
            SoulEngine::Logger::Log("Project closed, returning to welcome screen");
        }
        
        ImGui::End();
    }
    
    if (showSceneHierarchy) {
        ImGui::Begin("Scene Hierarchy", &showSceneHierarchy);
        ImGui::Text("Scene objects will be listed here");
        ImGui::End();
    }
    
    if (showInspector) {
        ImGui::Begin("Inspector", &showInspector);
        ImGui::Text("Object properties will be shown here");
        ImGui::End();
    }
    
    if (showConsole) {
        ImGui::Begin("Console", &showConsole);
        ImGui::Text("Engine logs and debug information");
        ImGui::End();
    }
    
    // 处理文件对话框（在主编辑器中可以允许正常的拖拽缩放）
    if (ImGuiFileDialog::Instance()->Display("OpenProjectDlg")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string sel = ImGuiFileDialog::Instance()->GetFilePathName();
            if (sel.empty()) sel = ImGuiFileDialog::Instance()->GetCurrentPath();
            if (!sel.empty()) OpenProjectAt(sel);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

// 辅助函数：重置Docking布局
void ResetDockingLayout() {
    static bool isFirstTime = true;
    if (isFirstTime && isProjectOpened && isDockingEnabled) {
        // ImGuiID dockspaceId = ImGui::GetID("EditorPanelDockSpace");
        // ImGui::DockBuilderRemoveNode(dockspaceId);
        // ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        // ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);
        
        // // 创建默认布局
        // ImGuiID dock_left, dock_right, dock_bottom;
        // ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.25f, &dock_left, &dock_right);
        // ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Down, 0.3f, &dock_bottom, &dock_right);
        
        // // 分配窗口到停靠位置
        // ImGui::DockBuilderDockWindow("Project Browser", dock_left);
        // ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_left);
        // ImGui::DockBuilderDockWindow("Inspector", dock_right);
        // ImGui::DockBuilderDockWindow("Console", dock_bottom);
        
        // ImGui::DockBuilderFinish(dockspaceId);
        isFirstTime = false;
    }
}

} // namespace

class MyApplication : public SoulEngine::ApplicationBase
{
public:
    MyApplication() : ApplicationBase("SoulEditor")
    {
    }

    std::shared_ptr<IProgram> program;
    std::shared_ptr<IVertexInputLayout> layout;
    std::shared_ptr<IBuffer> vertexBuffer1;
    std::shared_ptr<IBuffer> vertexBuffer2;
    std::shared_ptr<IBuffer> indexBuffer;

    bool OnInitialize() override
    {
        Logger::Log("MyApplication initialized");
        auto device = GetDevice();
        auto context = GetContext();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // 注意：不在初始化时启用Docking，而是动态控制
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);

    // Use appropriate GLSL version based on OpenGL version
#ifdef __APPLE__
    // macOS with OpenGL 3.3 Core Profile should use GLSL 330
    const char *glsl_version = "#version 330 core";
#else
    // Other platforms can use GLSL 130 or 330
    const char *glsl_version = "#version 130";
#endif

    ImGui_ImplOpenGL3_Init(glsl_version);

    // 禁用窗口resize功能
    GLFWwindow* window = glfwGetCurrentContext();
    if (window) {
        // 设置窗口大小限制，最小和最大都设为当前大小，实现禁用resize
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetWindowSizeLimits(window, width, height, width, height);
        
        SoulEngine::Logger::Log("Window resize disabled - fixed size: {}x{}", width, height);
    }
        engine->GetSystem<WindowSystem>()->GetWindow()->SetResizable(false);
        return true;
    }

    void Update(float deltaTime) override
    {

  
    }
    

    void Render() override
    {
   
    // 开始ImGui渲染
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // 动态控制Docking功能
    ImGuiIO &io = ImGui::GetIO();
    
    // 根据项目状态动态启用/禁用Docking
    if (isProjectOpened && !isDockingEnabled) {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        isDockingEnabled = true;
        SoulEngine::Logger::Log("Docking enabled - project opened");
    } else if (!isProjectOpened && isDockingEnabled) {
        io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
        isDockingEnabled = false;
        SoulEngine::Logger::Log("Docking disabled - showing welcome screen");
    }
    
    // 只有在项目打开且Docking启用时才创建DockSpace
    if (isProjectOpened && isDockingEnabled) {
        static ImGuiID dockspaceId = ImGui::GetID("EditorPanelDockSpace");
        ImGui::DockSpaceOverViewport(dockspaceId, ImGui::GetMainViewport());
        
        // 设置默认布局（仅第一次）
        ResetDockingLayout();
    }
    // ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // 根据项目状态显示不同的UI
    if (!isProjectOpened) {
        // 显示欢迎界面
        EditorWelcomeUI();
    } else {
        // 显示主编辑器界面
        ShowMainEditorUI();
    }
    
    // 可选：显示ImGui演示窗口用于调试
    // ImGui::ShowDemoWindow();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }


    }

    void Shutdown() override
    {
        SoulEngine::Logger::Log("MyApplication shutdown");
    }

    bool ShouldClose() const override
    {
        return GetWindow()->ShouldClose();
    }
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(MyApplication)
