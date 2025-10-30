#include "EditorPrefs.h"
#include "EngineFileIO.h"
#include "Logger.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <pwd.h>
#include <unistd.h>
#endif


namespace SoulEditor
{
    
    
    void EditorPrefs::Initialize()
    {
        if (initialized_)
            return;
        Load();
    }

    void EditorPrefs::Load()
    {
        try
        {
            auto filePath = GetSettingsFilePath();
            auto content = SoulEngine::EngineFileIO::LoadText(filePath);
            settings_ = nlohmann::json::parse(content);
            SoulEngine::Logger::Log("EditorPrefs loaded from {}",filePath);
        }
        catch (const std::exception& e)
        {
            SoulEngine::Logger::Error("EditorPrefs load failed: {}", e.what());
        }
    }

    void EditorPrefs::Save() const
    {
        try
        {
            auto filePath = GetSettingsFilePath();
            SoulEngine::EngineFileIO::SaveText(filePath, settings_.dump(4));
            SoulEngine::Logger::Log("EditorPrefs saved to {}",filePath);
        }
        catch (const std::exception& e)
        {
            SoulEngine::Logger::Error("EditorPrefs save failed: {}", e.what());
        }
    }

    template <class T>
    T EditorPrefs::Get(const std::string& key, const T& defaultValue)
    {
        if (settings_.contains(key))
        {
            try
            {
                return settings_[key].get<T>();
            }
            catch (...)
            {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    template <class T>
    void EditorPrefs::Set(const std::string& key, const T& value)
    {
        settings_[key] = value;
    }
    

    std::string EditorPrefs::GetString(const std::string& key, const std::string& defaultValue)
    {
        return Get<std::string>(key,defaultValue);
    }

    void EditorPrefs::SetString(const std::string& key, const std::string& value)
    {
        Set<std::string>(key,value);
    }

    int EditorPrefs::GetInt(const std::string& key, int defaultValue)
    {
        return Get<int>(key,defaultValue);
    }

    void EditorPrefs::SetInt(const std::string& key, int value)
    {
        Set<int>(key,value);
    }

    bool EditorPrefs::GetBool(const std::string& key, bool defaultValue)
    {
        return Get<bool>(key,defaultValue);
    }

    void EditorPrefs::SetBool(const std::string& key, bool value)
    {
        Set<bool>(key,value);
    }

    double EditorPrefs::GetDouble(const std::string& key, double defaultValue)
    {
        return Get<double>(key,defaultValue);
    }

    void EditorPrefs::SetDouble(const std::string& key, double value)
    {
        Set<double>(key,value);
    }

    bool EditorPrefs::HasKey(const std::string& key) const
    {
        return settings_.contains(key);
    }

    bool EditorPrefs::DeleteKey(const std::string& key)
    {
        if (HasKey(key))
        {
            settings_.erase(key);
            return true;
        }
        return false;
    }

    std::string EditorPrefs::GetSettingsFilePath()
    {
#ifdef _WIN32
        // Windows: %APPDATA%/SoulEngine/editor_settings.json
        char* appDataPath = nullptr;
        size_t len = 0;
        if (_dupenv_s(&appDataPath, &len, "APPDATA") == 0 && appDataPath != nullptr) 
        {
            std::string path = std::string(appDataPath) + "/SoulEngine";
            free(appDataPath);
            std::filesystem::create_directories(path);
            return path + "/editor_settings.json";
        }
        throw std::runtime_error("Failed to get APPDATA path");
            
#elif defined(__APPLE__)
        // macOS: ~/Library/Application Support/SoulEngine/editor_settings.json
        const char* homeDir = getpwuid(getuid())->pw_dir;
        std::string path = std::string(homeDir) + "/Library/Application Support/SoulEngine";
        std::filesystem::create_directories(path);
        return path + "/editor_settings.json";
            
#else
        throw std::runtime_error("Unsupported platform");
#endif
    }


    template int EditorPrefs::Get(const std::string& key, const int& defaultValue);
    template void EditorPrefs::Set(const std::string& key, const int& value);
    template double EditorPrefs::Get(const std::string& key, const double& defaultValue);
    template void EditorPrefs::Set(const std::string& key, const double& value);
    template bool EditorPrefs::Get(const std::string& key, const bool& defaultValue);
    template void EditorPrefs::Set(const std::string& key, const bool& value);
    template std::string EditorPrefs::Get(const std::string& key, const std::string& defaultValue);
    template void EditorPrefs::Set(const std::string& key, const std::string& value);
    
}
