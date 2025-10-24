#include "SoulEngine.h"
#include "Core/ApplicationHelper.h"
#include "Core/Timer.h"
#include "EngineFileIO.h"
#include "Input.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "ImGuiFileDialog.h"
#include <filesystem>
#include <fstream>
#include <vector>

using namespace SoulEngine;
using namespace SoulEngine::Gfx;

namespace{

static std::vector<std::string> recentProjects;
static std::string currentProjectRoot;

static void AddToRecent(const std::string &path) {
    // keep unique, most-recent-first, limit to 10
    recentProjects.erase(std::remove(recentProjects.begin(), recentProjects.end(), path), recentProjects.end());
    recentProjects.insert(recentProjects.begin(), path);
    if (recentProjects.size() > 10) recentProjects.resize(10);
}

static void CreateProjectAt(const std::string &root) {
    namespace fs = std::filesystem;
    try {
        fs::create_directories(fs::path(root) / "Assets");
        fs::create_directories(fs::path(root) / "Library");
        fs::create_directories(fs::path(root) / "StreamingAssets");
        // write minimal project.json
        fs::path projFile = fs::path(root) / "project.json";
        std::ofstream ofs(projFile);
        if (ofs) {
            ofs << "{\"name\":\"NewProject\",\"assets\":\"Assets\",\"library\":\"Library\"}";
            ofs.close();
        }
        AddToRecent(root);
        currentProjectRoot = root;
        SoulEngine::Logger::Log("Created project at {}", root);
    } catch (const std::exception &e) {
        SoulEngine::Logger::Log("Failed to create project at {}: {}", root, e.what());
    }
}

static void OpenProjectAt(const std::string &pathOrFile) {
    namespace fs = std::filesystem;
    fs::path p(pathOrFile);
    std::string root;
    if (fs::is_regular_file(p) && p.filename() == "project.json") {
        root = p.parent_path().string();
    } else if (fs::is_directory(p)) {
        root = p.string();
    } else if (fs::is_regular_file(p)) {
        // if user selected another file, use its parent as project root
        root = p.parent_path().string();
    }
    if (!root.empty()) {
        AddToRecent(root);
        currentProjectRoot = root;
        SoulEngine::Logger::Log("Opened project: {}", root);
        // TODO: Load project config and initialize ResourceManager
    } else {
        SoulEngine::Logger::Log("Invalid project path: {}", pathOrFile);
    }
}

void EditorWelcomeUI(){
    // 在 NewFrame 之后、DockSpace 之前或作为独立弹窗
    ImGui::SetNextWindowSize(ImVec2(600, 360), ImGuiCond_FirstUseEver);
    ImGui::Begin("Welcome to SoulEditor", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::TextWrapped("Welcome — choose an action:");

    if (ImGui::Button("Create Project", ImVec2(200, 0))) {
        // open folder picker (select a directory to create project in)
        ImGuiFileDialog::Instance()->OpenDialog("CreateProjectDlg", "Select Project Folder",  ".");
    }

    ImGui::SameLine();
    if (ImGui::Button("Open Project", ImVec2(200, 0))) {
        // open file dialog to choose project.json or project folder
        ImGuiFileDialog::Instance()->OpenDialog("OpenProjectDlg", "Open Project", "project.json,.*");
    }

    ImGui::Separator();
    ImGui::Text("Recent Projects:");
    for (size_t i = 0; i < recentProjects.size(); ++i) {
        const auto &p = recentProjects[i];
        ImGui::PushID((int)i);
        if (ImGui::Button(p.c_str())) { OpenProjectAt(p); }
        ImGui::PopID();
    }

    // Handle CreateProject dialog result
    if (ImGuiFileDialog::Instance()->Display("CreateProjectDlg")) {
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

    // Handle OpenProject dialog result
    if (ImGuiFileDialog::Instance()->Display("OpenProjectDlg")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string sel = ImGuiFileDialog::Instance()->GetFilePathName();
            if (sel.empty()) sel = ImGuiFileDialog::Instance()->GetCurrentPath();
            if (!sel.empty()) OpenProjectAt(sel);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::End();
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
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
    // ImGuizmo::BeginFrame(); // 移到这里，每帧开始时调用
    static ImGuiID id = ImGui::GetID("EditorPanelDockSpace");
    ImGui::DockSpaceOverViewport(id, ImGui::GetMainViewport());
    ImGuiIO &io = ImGui::GetIO();
    // ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    // show imgui demo
    // ImGui::ShowDemoWindow();
EditorWelcomeUI();

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
