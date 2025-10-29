#pragma once
#include "Define.h"
#include <string>
#include "Project.h"
namespace SoulEditor
{
    class ProjectManager
    {
        SINGLETON_CLASS(ProjectManager)
    public:
        bool CreateProject(const std::string& path,const std::string& name);
        bool OpenProject(const std::string& path);

    private:
    };

}
