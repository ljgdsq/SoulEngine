#include "Project.h"
#include <fstream>
#include <filesystem>

namespace SoulEngine
{
    bool Project::CreateNewProject(const std::string &path, const std::string &name)
    {
        // 设置项目基本信息
        projectPath_ = path;
        settings_.name = name;
        settings_.description = "";
        
        // 创建项目目录结构
        if (!CreateProjectDirectories())
        {
            return false;
        }
        
        // 创建项目文件
        if (!CreateProjectFile())
        {
            return false;
        }
        
        isLoaded_ = true;
        isDirty_ = false;
        return true;
    }

    bool Project::LoadProject(const std::string &path)
    {
        // 检查路径是否为项目文件还是项目目录
        std::filesystem::path fsPath(path);
        
        if (std::filesystem::is_directory(fsPath))
        {
            // 如果是目录，查找项目文件
            projectPath_ = path;
            std::filesystem::path projectFile = fsPath / "project.json";
            if (!std::filesystem::exists(projectFile))
            {
                return false;
            }
        }
        else if (std::filesystem::is_regular_file(fsPath))
        {
            // 如果是文件，获取父目录作为项目路径
            projectPath_ = fsPath.parent_path().string();
        }
        else
        {
            return false;
        }
        
        // 加载项目文件
        if (!LoadProjectFile())
        {
            return false;
        }
        
        isLoaded_ = true;
        isDirty_ = false;
        return true;
    }

    bool Project::SaveProject()
    {
        if (!isLoaded_)
        {
            return false;
        }
        
        if (SaveProjectFile())
        {
            MarkClean();
            return true;
        }
        
        return false;
    }

    void Project::CloseProject()
    {
        projectPath_.clear();
        settings_ = ProjectSettings();
        isLoaded_ = false;
        isDirty_ = false;
    }

    bool Project::CreateProjectDirectories()
    {
        try
        {
            // 创建主要目录
            std::filesystem::create_directories(projectPath_);
            std::filesystem::create_directories(GetAssetsPath());
            std::filesystem::create_directories(GetLibraryPath());
            std::filesystem::create_directories(GetLogsPath());
            
            return true;
        }
        catch (const std::filesystem::filesystem_error&)
        {
            return false;
        }
    }

    bool Project::CreateProjectFile()
    {
        return SaveProjectFile();
    }

    bool Project::LoadProjectFile()
    {
        std::filesystem::path projectFile = std::filesystem::path(projectPath_) / "project.json";
        
        std::ifstream file(projectFile);
        if (!file.is_open())
        {
            return false;
        }
        
        // 简单的键值对解析
        std::string line;
        while (std::getline(file, line))
        {
            // 跳过空行和注释
            if (line.empty() || line[0] == '#' || line[0] == '/')
                continue;
                
            size_t pos = line.find('=');
            if (pos != std::string::npos)
            {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // 去除前后空格
                key.erase(0, key.find_first_not_of(" \t\""));
                key.erase(key.find_last_not_of(" \t\"") + 1);
                value.erase(0, value.find_first_not_of(" \t\""));
                value.erase(value.find_last_not_of(" \t\"") + 1);
                
                if (key == "name")
                {
                    settings_.name = value;
                }
                else if (key == "version")
                {
                    settings_.version = value;
                }
                else if (key == "description")
                {
                    settings_.description = value;
                }
            }
        }
        
        file.close();
        return true;
    }

    bool Project::SaveProjectFile()
    {
        std::filesystem::path projectFile = std::filesystem::path(projectPath_) / "project.json";
        
        std::ofstream file(projectFile);
        if (!file.is_open())
        {
            return false;
        }
        
        // 写入简单的键值对格式
        file << "# SoulEngine Project File\n";
        file << "name=" << settings_.name << "\n";
        file << "version=" << settings_.version << "\n";
        file << "description=" << settings_.description << "\n";
        
        file.close();
        return true;
    }

} // namespace SoulEngine