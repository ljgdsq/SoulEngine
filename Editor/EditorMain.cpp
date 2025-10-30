#include "ApplicationHelper.h"
#include "Engine.h"
#include <memory>

#include "GuiManager.h"
#include "Input.h"
#include "Project/ProjectManager.h"
#include "Window/WindowSystem.h"

class EditorApplication : public SoulEngine::ApplicationBase
{
public:
    bool OnInitialize() override
    {
        m_window->SetResizable(false);
        m_window->SetTitle("SoulEditor");
        m_guiManager = std::make_unique<SoulEditor::GuiManager>();
        m_guiManager->Initialize(static_cast<GLFWwindow*>(m_window->GetNativeWindowHandle()));
        SoulEditor::ProjectManager::GetInstance().SetOnProjectOpenedCallback([this]()
        {
            m_window->SetResizable(true);
            m_window->SetSize(1280,720);
            m_window->SetTitle("SoulEditor - "+SoulEditor::ProjectManager::GetInstance().GetCurrentProject()->GetProjectName() );
            m_guiManager->SwitchToMainEditor();
        });
        SoulEditor::ProjectManager::GetInstance().SetOnProjectCreatedCallback([this]()
        {
            m_window->SetResizable(true);
            m_window->SetSize(1280,720);
            m_window->SetTitle("SoulEditor - "+SoulEditor::ProjectManager::GetInstance().GetCurrentProject()->GetProjectName() );
            m_guiManager->SwitchToMainEditor();
        });

        m_guiManager->SwitchToStartupScreen();
        return true;
    }
    void Update(float deltaTime) override
    {
        if (SoulEngine::Input::GetKeyDown(SoulEngine::KeyCode::Escape))
        {
           m_window->SetShouldClose(true);
        }
    }
    void Render() override
    {
        m_guiManager->BeginFrame();
        m_guiManager->Render();
        m_guiManager->EndFrame();
    }
    void Shutdown() override{}
    ~EditorApplication() override = default;

private:
    std::unique_ptr<SoulEditor::GuiManager> m_guiManager;
};



SOULENGINE_MAIN(EditorApplication);
