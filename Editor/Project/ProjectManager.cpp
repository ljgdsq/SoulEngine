#include "ProjectManager.h"

namespace SoulEditor
{
    bool ProjectManager::CreateProject(const std::string& path, const std::string& name)
    {
        auto proj= std::make_shared<SoulEngine::Project>();
        proj->CreateNewProject(path,name);
        return true;
    }

    bool ProjectManager::OpenProject(const std::string& path)
    {
        auto proj = std::make_shared<SoulEngine::Project>();
        proj->LoadProject(path);
        return true;
    }

}

